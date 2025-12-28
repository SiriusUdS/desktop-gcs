#include "LaunchWindow.h"

#include "ImGuiConfig.h"
#include "MonitoringWindow.h"

#include <imgui.h>

const char* const LaunchWindow::name = "Launch";

using StateRect = StateMachineRenderer::StateRect;
using Arrow = StateMachineRenderer::Arrow;
using ArrowPathType = StateMachineRenderer::ArrowPathType;
using AnchorEdgeSide = StateMachineRenderer::AnchorEdgeSide;
using ArrowheadDir = StateMachineRenderer::ArrowheadDir;
using Label = StateMachineRenderer::Label;

const char* LaunchWindow::getName() const {
    return name;
}

const char* LaunchWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void LaunchWindow::lazyInit() {
    initFillStationStateMachine();
    initMotorStateMachine();
}

void LaunchWindow::renderImpl() {
    ImGui::SeparatorText("State Machines");

    constexpr float stateMachineHeight = 600.0f;

    if (ImGui::BeginTable("Plots", 2, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Filling station control state machine");
        ImGui::TableSetupColumn("Engine control state machine");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        fsStateMachine.render({-1.0f, stateMachineHeight});

        ImGui::TableSetColumnIndex(1);
        motorStateMachine.render({-1.0f, stateMachineHeight});

        ImGui::EndTable();
    }

    if (!inLaunchState && motorFireStateActive) {
        inLaunchState = true;
        ImGui::SetWindowFocus(MonitoringWindow::name);
    } else if (!motorFireStateActive) {
        inLaunchState = false;
    }

    ImGui::TextWrapped(
      "* Once the Engine control state machine has reached the FIRE state, the application will automatically switch to the Monitoring window.");
}

void LaunchWindow::initFillStationStateMachine() {
    fsStateMachine.params.showArrowLabels = false;

    ImVec2 rectSize{180, 40};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect initState{.position{-350, 0}, .size{rectSize}, .active{fsInitStateActive}, .label = "INIT"};
    StateRect safeState{.position{0, 0}, .size{rectSize}, .active{fsSafeStateActive}, .label = "SAFE"};
    StateRect testState{.position{0, -110}, .size{rectSize}, .active{fsTestStateActive}, .label = "TEST"};
    StateRect abortState{.position{300, -110}, .size{rectSize}, .active{fsAbortStateActive}, .label = "ABORT"};
    StateRect errorState{.position{-175, 110}, .size{rectSize}, .active{fsErrorStateActive}, .label = "ERROR"};
    StateRect unsafeState{.position{175, 110}, .size{rectSize}, .active{fsUnsafeStateActive}, .label = "UNSAFE"};
    StateRect igniteState{.position{175, 220}, .size{rectSize}, .active{fsIgniteStateActive}, .label = "IGNITE"};

    Label testToErrorLabel{.position{-300, -110}, .text = "To ERROR"};
    Label igniteToSafeLabel{.position{-75, 220}, .text = "To SAFE"};

    Arrow initToSafeArrow = fsStateMachine.createArrow(initState, {AnchorEdgeSide::RIGHT}, safeState, {AnchorEdgeSide::LEFT});
    initToSafeArrow.label = "[Init Completed]";

    Arrow initToErrorArrow = fsStateMachine.createArrow(initState, {AnchorEdgeSide::BOTTOM}, errorState, {AnchorEdgeSide::TOP, 0.25f});
    initToErrorArrow.label = "[Init Failed]";

    Arrow safeToTestArrow = fsStateMachine.createArrow(safeState, {AnchorEdgeSide::TOP}, testState, {AnchorEdgeSide::BOTTOM});
    safeToTestArrow.label = "TEST";

    Arrow safeToUnsafeArrow = fsStateMachine.createArrow(safeState, {AnchorEdgeSide::BOTTOM, 0.25f}, unsafeState, {AnchorEdgeSide::TOP, 0.25f});
    safeToUnsafeArrow.label = "UNSAFE";

    Arrow safeToErrorArrow =
      fsStateMachine.createArrow(safeState, {AnchorEdgeSide::BOTTOM}, errorState, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);

    Arrow safeToAbortArrow =
      fsStateMachine.createArrow(safeState, {AnchorEdgeSide::TOP, 0.85f}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 20);

    Arrow testToSafeArrow = fsStateMachine.createArrow(testState, {AnchorEdgeSide::BOTTOM, 0.85f}, safeState, {AnchorEdgeSide::TOP, 0.15f});
    testToSafeArrow.label = "SAFE";

    Arrow testToErrorArrow = fsStateMachine.createArrow(testState, {AnchorEdgeSide::LEFT}, testToErrorLabel, {AnchorEdgeSide::RIGHT});
    testToErrorArrow.label = "[On Error]";

    Arrow abortToSafeArrow = fsStateMachine.createArrow(abortState,
                                                        {AnchorEdgeSide::BOTTOM, 0.85f},
                                                        safeState,
                                                        {AnchorEdgeSide::TOP, 0.70f},
                                                        ArrowPathType::HORIZONTAL,
                                                        -10);
    abortToSafeArrow.label = "SAFE";

    Arrow errorToSafeArrow = fsStateMachine.createArrow(errorState, {AnchorEdgeSide::TOP, 0.75f}, safeState, {AnchorEdgeSide::BOTTOM, 0.75f});
    errorToSafeArrow.label = "SAFE";

    Arrow errorToAbortArrow =
      fsStateMachine.createArrow(errorState, {AnchorEdgeSide::BOTTOM}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 275.0f);

    Arrow unsafeToSafeArrow =
      fsStateMachine.createArrow(unsafeState, {AnchorEdgeSide::TOP}, safeState, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);
    unsafeToSafeArrow.label = "SAFE";

    Arrow unsafeToIgniteArrow = fsStateMachine.createArrow(unsafeState, {AnchorEdgeSide::BOTTOM}, igniteState, {AnchorEdgeSide::TOP});
    unsafeToIgniteArrow.label = "IGNITE";

    Arrow unsafeToErrorArrow = fsStateMachine.createArrow(unsafeState, {AnchorEdgeSide::LEFT}, errorState, {AnchorEdgeSide::RIGHT});
    unsafeToErrorArrow.label = "[On Error]";

    Arrow unsafeToAbortArrow =
      fsStateMachine.createArrow(unsafeState, {AnchorEdgeSide::RIGHT}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);
    unsafeToAbortArrow.label = "ABORT";
    unsafeToAbortArrow.labelOffset = {0, -145};

    Arrow igniteToSafeArrow = fsStateMachine.createArrow(igniteState, {AnchorEdgeSide::LEFT}, igniteToSafeLabel, {AnchorEdgeSide::RIGHT});
    igniteToSafeArrow.label = "SAFE";

    fsStateMachine.addStateRect(initState);
    fsStateMachine.addStateRect(safeState);
    fsStateMachine.addStateRect(testState);
    fsStateMachine.addStateRect(abortState);
    fsStateMachine.addStateRect(errorState);
    fsStateMachine.addStateRect(unsafeState);
    fsStateMachine.addStateRect(igniteState);
    fsStateMachine.addLabel(testToErrorLabel);
    fsStateMachine.addLabel(igniteToSafeLabel);
    fsStateMachine.addArrow(initToSafeArrow);
    fsStateMachine.addArrow(initToErrorArrow);
    fsStateMachine.addArrow(safeToTestArrow);
    fsStateMachine.addArrow(safeToUnsafeArrow);
    fsStateMachine.addArrow(safeToErrorArrow);
    fsStateMachine.addArrow(safeToAbortArrow);
    fsStateMachine.addArrow(testToSafeArrow);
    fsStateMachine.addArrow(testToErrorArrow);
    fsStateMachine.addArrow(abortToSafeArrow);
    fsStateMachine.addArrow(errorToSafeArrow);
    fsStateMachine.addArrow(errorToAbortArrow);
    fsStateMachine.addArrow(unsafeToSafeArrow);
    fsStateMachine.addArrow(unsafeToIgniteArrow);
    fsStateMachine.addArrow(unsafeToErrorArrow);
    fsStateMachine.addArrow(unsafeToAbortArrow);
    fsStateMachine.addArrow(igniteToSafeArrow);
}

void LaunchWindow::initMotorStateMachine() {
    motorStateMachine.params.showArrowLabels = false;

    ImVec2 rectSize{180, 40};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect initState{.position{-350, 0}, .size{rectSize}, .active{motorInitStateActive}, .label = "INIT"};
    StateRect safeState{.position{0, 0}, .size{rectSize}, .active{motorSafeStateActive}, .label = "SAFE"};
    StateRect testState{.position{0, -110}, .size{rectSize}, .active{motorTestStateActive}, .label = "TEST"};
    StateRect abortState{.position{300, -110}, .size{rectSize}, .active{motorAbortStateActive}, .label = "ABORT"};
    StateRect errorState{.position{-175, 110}, .size{rectSize}, .active{motorErrorStateActive}, .label = "ERROR"};
    StateRect unsafeState{.position{175, 110}, .size{rectSize}, .active{motorUnsafeStateActive}, .label = "UNSAFE"};
    StateRect igniteState{.position{175, 220}, .size{rectSize}, .active{motorIgniteStateActive}, .label = "IGNITE"};
    StateRect fireState{.position{175, 330}, .size{rectSize}, .active{motorFireStateActive}, .label = "FIRE"};

    Label testToErrorLabel{.position{-300, -110}, .text = "To ERROR"};
    Label igniteToSafeLabel{.position{-75, 220}, .text = "To SAFE"};
    Label fireToSafeLabel{.position{-75, 330}, .text = "To SAFE"};

    Arrow initToSafeArrow = motorStateMachine.createArrow(initState, {AnchorEdgeSide::RIGHT}, safeState, {AnchorEdgeSide::LEFT});
    initToSafeArrow.label = "[Init Completed]";

    Arrow initToErrorArrow = motorStateMachine.createArrow(initState, {AnchorEdgeSide::BOTTOM}, errorState, {AnchorEdgeSide::TOP, 0.25f});
    initToErrorArrow.label = "[Init Failed]";

    Arrow safeToTestArrow = motorStateMachine.createArrow(safeState, {AnchorEdgeSide::TOP}, testState, {AnchorEdgeSide::BOTTOM});
    safeToTestArrow.label = "TEST";

    Arrow safeToUnsafeArrow = motorStateMachine.createArrow(safeState, {AnchorEdgeSide::BOTTOM, 0.25f}, unsafeState, {AnchorEdgeSide::TOP, 0.25f});
    safeToUnsafeArrow.label = "UNSAFE";

    Arrow safeToErrorArrow =
      motorStateMachine.createArrow(safeState, {AnchorEdgeSide::BOTTOM}, errorState, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);

    Arrow safeToAbortArrow =
      motorStateMachine.createArrow(safeState, {AnchorEdgeSide::TOP, 0.85f}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 20);

    Arrow testToSafeArrow = motorStateMachine.createArrow(testState, {AnchorEdgeSide::BOTTOM, 0.85f}, safeState, {AnchorEdgeSide::TOP, 0.15f});
    testToSafeArrow.label = "SAFE";

    Arrow testToErrorArrow = motorStateMachine.createArrow(testState, {AnchorEdgeSide::LEFT}, testToErrorLabel, {AnchorEdgeSide::RIGHT});
    testToErrorArrow.label = "[On Error]";

    Arrow abortToSafeArrow = motorStateMachine.createArrow(abortState,
                                                           {AnchorEdgeSide::BOTTOM, 0.85f},
                                                           safeState,
                                                           {AnchorEdgeSide::TOP, 0.70f},
                                                           ArrowPathType::HORIZONTAL,
                                                           -10);
    abortToSafeArrow.label = "SAFE";

    Arrow errorToSafeArrow = motorStateMachine.createArrow(errorState, {AnchorEdgeSide::TOP, 0.75f}, safeState, {AnchorEdgeSide::BOTTOM, 0.75f});
    errorToSafeArrow.label = "SAFE";

    Arrow errorToAbortArrow =
      motorStateMachine.createArrow(errorState, {AnchorEdgeSide::BOTTOM}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 385.0f);

    Arrow unsafeToSafeArrow =
      motorStateMachine.createArrow(unsafeState, {AnchorEdgeSide::TOP}, safeState, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);
    unsafeToSafeArrow.label = "SAFE";

    Arrow unsafeToIgniteArrow = motorStateMachine.createArrow(unsafeState, {AnchorEdgeSide::BOTTOM}, igniteState, {AnchorEdgeSide::TOP});
    unsafeToIgniteArrow.label = "IGNITE";

    Arrow unsafeToErrorArrow = motorStateMachine.createArrow(unsafeState, {AnchorEdgeSide::LEFT}, errorState, {AnchorEdgeSide::RIGHT});
    unsafeToErrorArrow.label = "[On Error]";

    Arrow unsafeToAbortArrow =
      motorStateMachine.createArrow(unsafeState, {AnchorEdgeSide::RIGHT}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);
    unsafeToAbortArrow.label = "ABORT";
    unsafeToAbortArrow.labelOffset = {0, -145};

    Arrow igniteToSafeArrow = motorStateMachine.createArrow(igniteState, {AnchorEdgeSide::LEFT}, igniteToSafeLabel, {AnchorEdgeSide::RIGHT});
    igniteToSafeArrow.label = "SAFE";

    Arrow igniteToAbortArrow =
      motorStateMachine.createArrow(igniteState, {AnchorEdgeSide::RIGHT}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);

    Arrow igniteToFireArrow = motorStateMachine.createArrow(igniteState, {AnchorEdgeSide::BOTTOM}, fireState, {AnchorEdgeSide::TOP});
    igniteToFireArrow.label = "FIRE";

    Arrow fireToSafeArrow = motorStateMachine.createArrow(fireState, {AnchorEdgeSide::LEFT}, fireToSafeLabel, {AnchorEdgeSide::RIGHT});
    igniteToSafeArrow.label = "SAFE";

    Arrow fireToAbortArrow =
      motorStateMachine.createArrow(fireState, {AnchorEdgeSide::RIGHT}, abortState, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);

    motorStateMachine.addStateRect(initState);
    motorStateMachine.addStateRect(safeState);
    motorStateMachine.addStateRect(testState);
    motorStateMachine.addStateRect(abortState);
    motorStateMachine.addStateRect(errorState);
    motorStateMachine.addStateRect(unsafeState);
    motorStateMachine.addStateRect(igniteState);
    motorStateMachine.addStateRect(fireState);
    motorStateMachine.addLabel(testToErrorLabel);
    motorStateMachine.addLabel(igniteToSafeLabel);
    motorStateMachine.addLabel(fireToSafeLabel);
    motorStateMachine.addArrow(initToSafeArrow);
    motorStateMachine.addArrow(initToErrorArrow);
    motorStateMachine.addArrow(safeToTestArrow);
    motorStateMachine.addArrow(safeToUnsafeArrow);
    motorStateMachine.addArrow(safeToErrorArrow);
    motorStateMachine.addArrow(safeToAbortArrow);
    motorStateMachine.addArrow(testToSafeArrow);
    motorStateMachine.addArrow(testToErrorArrow);
    motorStateMachine.addArrow(abortToSafeArrow);
    motorStateMachine.addArrow(errorToSafeArrow);
    motorStateMachine.addArrow(errorToAbortArrow);
    motorStateMachine.addArrow(unsafeToSafeArrow);
    motorStateMachine.addArrow(unsafeToIgniteArrow);
    motorStateMachine.addArrow(unsafeToErrorArrow);
    motorStateMachine.addArrow(unsafeToAbortArrow);
    motorStateMachine.addArrow(igniteToSafeArrow);
    motorStateMachine.addArrow(igniteToAbortArrow);
    motorStateMachine.addArrow(igniteToFireArrow);
    motorStateMachine.addArrow(fireToSafeArrow);
    motorStateMachine.addArrow(fireToAbortArrow);
}

#include "LaunchWindow.h"

#include "ImGuiConfig.h"

#include <algorithm>
#include <imgui.h>

using StateRect = StateMachineRenderer::StateRect;
using Arrow = StateMachineRenderer::Arrow;
using ArrowPathType = StateMachineRenderer::ArrowPathType;
using AnchorEdgeSide = StateMachineRenderer::AnchorEdgeSide;
using ArrowheadDir = StateMachineRenderer::ArrowheadDir;
using Label = StateMachineRenderer::Label;

void LaunchWindow::lazyInit() {
    initFillStationStateMachine();
    initMotorStateMachine();
}

void LaunchWindow::renderImpl() {
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    float stateMachineHeight = 600.0f;

    if (ImGui::BeginTable("Plots", 2)) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        fsStateMachine.render({windowSize.x / 2, stateMachineHeight});

        ImGui::TableSetColumnIndex(1);
        motorStateMachine.render({windowSize.x / 2, stateMachineHeight});

        ImGui::EndTable();
    }
}

const char* LaunchWindow::name() const {
    return "Launch";
}

const char* LaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void LaunchWindow::initFillStationStateMachine() {
    fsStateMachine.params.showArrowLabels = false;

    ImVec2 rectSize{180, 40};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect fsInit{.position{-350, 0}, .size{rectSize}, .active{false}, .label{"INIT"}};
    StateRect fsSafe{.position{0, 0}, .size{rectSize}, .active{false}, .label{"SAFE"}};
    StateRect fsTest{.position{0, -110}, .size{rectSize}, .active{false}, .label{"TEST"}};
    StateRect fsAbort{.position{300, -110}, .size{rectSize}, .active{false}, .label{"ABORT"}};
    StateRect fsError{.position{-175, 110}, .size{rectSize}, .active{true}, .label{"ERROR"}};
    StateRect fsUnsafe{.position{175, 110}, .size{rectSize}, .active{false}, .label{"UNSAFE"}};
    StateRect fsIgnite{.position{175, 220}, .size{rectSize}, .active{false}, .label{"IGNITE"}};

    Label fsTestToErrorLabel{.position{-300, -110}, .text{"To ERROR"}};
    Label fsIgniteToSafeLabel{.position{-75, 220}, .text{"To SAFE"}};

    Arrow fsInitToSafe = fsStateMachine.createArrow(fsInit, {AnchorEdgeSide::RIGHT}, fsSafe, {AnchorEdgeSide::LEFT});
    fsInitToSafe.label = "[Init Completed]";

    Arrow fsInitToError = fsStateMachine.createArrow(fsInit, {AnchorEdgeSide::BOTTOM}, fsError, {AnchorEdgeSide::TOP, 0.25f});
    fsInitToError.label = "[Init Failed]";

    Arrow fsSafeToTest = fsStateMachine.createArrow(fsSafe, {AnchorEdgeSide::TOP}, fsTest, {AnchorEdgeSide::BOTTOM});
    fsSafeToTest.label = "TEST";

    Arrow fsSafeToUnsafe = fsStateMachine.createArrow(fsSafe, {AnchorEdgeSide::BOTTOM, 0.25f}, fsUnsafe, {AnchorEdgeSide::TOP, 0.25f});
    fsSafeToUnsafe.label = "UNSAFE";

    Arrow fsSafeToError = fsStateMachine.createArrow(fsSafe, {AnchorEdgeSide::BOTTOM}, fsError, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);

    Arrow fsSafeToAbort =
      fsStateMachine.createArrow(fsSafe, {AnchorEdgeSide::TOP, 0.85f}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 20);

    Arrow fsTestToSafe = fsStateMachine.createArrow(fsTest, {AnchorEdgeSide::BOTTOM, 0.85f}, fsSafe, {AnchorEdgeSide::TOP, 0.15f});
    fsTestToSafe.label = "SAFE";

    Arrow fsTestToError = fsStateMachine.createArrow(fsTest, {AnchorEdgeSide::LEFT}, fsTestToErrorLabel, {AnchorEdgeSide::RIGHT});
    fsTestToError.label = "[On Error]";

    Arrow fsAbortToSafe =
      fsStateMachine.createArrow(fsAbort, {AnchorEdgeSide::BOTTOM, 0.85f}, fsSafe, {AnchorEdgeSide::TOP, 0.70f}, ArrowPathType::HORIZONTAL, -10);
    fsAbortToSafe.label = "SAFE";

    Arrow fsErrorToSafe = fsStateMachine.createArrow(fsError, {AnchorEdgeSide::TOP, 0.75f}, fsSafe, {AnchorEdgeSide::BOTTOM, 0.75f});
    fsErrorToSafe.label = "SAFE";

    Arrow fsErrorToAbort =
      fsStateMachine.createArrow(fsError, {AnchorEdgeSide::BOTTOM}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 275.0f);

    Arrow fsUnsafeToSafe = fsStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::TOP}, fsSafe, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToSafe.label = "SAFE";

    Arrow fsUnsafeToIgnite = fsStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::BOTTOM}, fsIgnite, {AnchorEdgeSide::TOP});
    fsUnsafeToIgnite.label = "IGNITE";

    Arrow fsUnsafeToError = fsStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::LEFT}, fsError, {AnchorEdgeSide::RIGHT});
    fsUnsafeToError.label = "[On Error]";

    Arrow fsUnsafeToAbort =
      fsStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::RIGHT}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToAbort.label = "ABORT";
    fsUnsafeToAbort.labelOffset = {0, -145};

    Arrow fsIgniteToSafe = fsStateMachine.createArrow(fsIgnite, {AnchorEdgeSide::LEFT}, fsIgniteToSafeLabel, {AnchorEdgeSide::RIGHT});
    fsIgniteToSafe.label = "SAFE";

    fsStateMachine.addStateRect(fsInit);
    fsStateMachine.addStateRect(fsSafe);
    fsStateMachine.addStateRect(fsTest);
    fsStateMachine.addStateRect(fsAbort);
    fsStateMachine.addStateRect(fsError);
    fsStateMachine.addStateRect(fsUnsafe);
    fsStateMachine.addStateRect(fsIgnite);
    fsStateMachine.addLabel(fsTestToErrorLabel);
    fsStateMachine.addLabel(fsIgniteToSafeLabel);
    fsStateMachine.addArrow(fsInitToSafe);
    fsStateMachine.addArrow(fsInitToError);
    fsStateMachine.addArrow(fsSafeToTest);
    fsStateMachine.addArrow(fsSafeToUnsafe);
    fsStateMachine.addArrow(fsSafeToError);
    fsStateMachine.addArrow(fsSafeToAbort);
    fsStateMachine.addArrow(fsTestToSafe);
    fsStateMachine.addArrow(fsTestToError);
    fsStateMachine.addArrow(fsAbortToSafe);
    fsStateMachine.addArrow(fsErrorToSafe);
    fsStateMachine.addArrow(fsErrorToAbort);
    fsStateMachine.addArrow(fsUnsafeToSafe);
    fsStateMachine.addArrow(fsUnsafeToIgnite);
    fsStateMachine.addArrow(fsUnsafeToError);
    fsStateMachine.addArrow(fsUnsafeToAbort);
    fsStateMachine.addArrow(fsIgniteToSafe);
}

void LaunchWindow::initMotorStateMachine() {
    motorStateMachine.params.showArrowLabels = false;

    ImVec2 rectSize{180, 40};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect fsInit{.position{-350, 0}, .size{rectSize}, .active{false}, .label{"INIT"}};
    StateRect fsSafe{.position{0, 0}, .size{rectSize}, .active{false}, .label{"SAFE"}};
    StateRect fsTest{.position{0, -110}, .size{rectSize}, .active{false}, .label{"TEST"}};
    StateRect fsAbort{.position{300, -110}, .size{rectSize}, .active{false}, .label{"ABORT"}};
    StateRect fsError{.position{-175, 110}, .size{rectSize}, .active{true}, .label{"ERROR"}};
    StateRect fsUnsafe{.position{175, 110}, .size{rectSize}, .active{false}, .label{"UNSAFE"}};
    StateRect fsIgnite{.position{175, 220}, .size{rectSize}, .active{false}, .label{"IGNITE"}};
    StateRect fsFire{.position{175, 330}, .size{rectSize}, .active{false}, .label{"FIRE"}};

    Label fsTestToErrorLabel{.position{-300, -110}, .text{"To ERROR"}};
    Label fsIgniteToSafeLabel{.position{-75, 220}, .text{"To SAFE"}};
    Label fsFireToSafeLabel{.position{-75, 330}, .text{"To SAFE"}};

    Arrow fsInitToSafe = motorStateMachine.createArrow(fsInit, {AnchorEdgeSide::RIGHT}, fsSafe, {AnchorEdgeSide::LEFT});
    fsInitToSafe.label = "[Init Completed]";

    Arrow fsInitToError = motorStateMachine.createArrow(fsInit, {AnchorEdgeSide::BOTTOM}, fsError, {AnchorEdgeSide::TOP, 0.25f});
    fsInitToError.label = "[Init Failed]";

    Arrow fsSafeToTest = motorStateMachine.createArrow(fsSafe, {AnchorEdgeSide::TOP}, fsTest, {AnchorEdgeSide::BOTTOM});
    fsSafeToTest.label = "TEST";

    Arrow fsSafeToUnsafe = motorStateMachine.createArrow(fsSafe, {AnchorEdgeSide::BOTTOM, 0.25f}, fsUnsafe, {AnchorEdgeSide::TOP, 0.25f});
    fsSafeToUnsafe.label = "UNSAFE";

    Arrow fsSafeToError =
      motorStateMachine.createArrow(fsSafe, {AnchorEdgeSide::BOTTOM}, fsError, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);

    Arrow fsSafeToAbort =
      motorStateMachine.createArrow(fsSafe, {AnchorEdgeSide::TOP, 0.85f}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 20);

    Arrow fsTestToSafe = motorStateMachine.createArrow(fsTest, {AnchorEdgeSide::BOTTOM, 0.85f}, fsSafe, {AnchorEdgeSide::TOP, 0.15f});
    fsTestToSafe.label = "SAFE";

    Arrow fsTestToError = motorStateMachine.createArrow(fsTest, {AnchorEdgeSide::LEFT}, fsTestToErrorLabel, {AnchorEdgeSide::RIGHT});
    fsTestToError.label = "[On Error]";

    Arrow fsAbortToSafe =
      motorStateMachine.createArrow(fsAbort, {AnchorEdgeSide::BOTTOM, 0.85f}, fsSafe, {AnchorEdgeSide::TOP, 0.70f}, ArrowPathType::HORIZONTAL, -10);
    fsAbortToSafe.label = "SAFE";

    Arrow fsErrorToSafe = motorStateMachine.createArrow(fsError, {AnchorEdgeSide::TOP, 0.75f}, fsSafe, {AnchorEdgeSide::BOTTOM, 0.75f});
    fsErrorToSafe.label = "SAFE";

    Arrow fsErrorToAbort =
      motorStateMachine.createArrow(fsError, {AnchorEdgeSide::BOTTOM}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 385.0f);

    Arrow fsUnsafeToSafe = motorStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::TOP}, fsSafe, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToSafe.label = "SAFE";

    Arrow fsUnsafeToIgnite = motorStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::BOTTOM}, fsIgnite, {AnchorEdgeSide::TOP});
    fsUnsafeToIgnite.label = "IGNITE";

    Arrow fsUnsafeToError = motorStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::LEFT}, fsError, {AnchorEdgeSide::RIGHT});
    fsUnsafeToError.label = "[On Error]";

    Arrow fsUnsafeToAbort =
      motorStateMachine.createArrow(fsUnsafe, {AnchorEdgeSide::RIGHT}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToAbort.label = "ABORT";
    fsUnsafeToAbort.labelOffset = {0, -145};

    Arrow fsIgniteToSafe = motorStateMachine.createArrow(fsIgnite, {AnchorEdgeSide::LEFT}, fsIgniteToSafeLabel, {AnchorEdgeSide::RIGHT});
    fsIgniteToSafe.label = "SAFE";

    Arrow fsIgniteToAbort =
      motorStateMachine.createArrow(fsIgnite, {AnchorEdgeSide::RIGHT}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);

    Arrow fsIgniteToFire = motorStateMachine.createArrow(fsIgnite, {AnchorEdgeSide::BOTTOM}, fsFire, {AnchorEdgeSide::TOP});
    fsIgniteToFire.label = "FIRE";

    Arrow fsFireToSafe = motorStateMachine.createArrow(fsFire, {AnchorEdgeSide::LEFT}, fsFireToSafeLabel, {AnchorEdgeSide::RIGHT});
    fsIgniteToSafe.label = "SAFE";

    Arrow fsFireToAbort =
      motorStateMachine.createArrow(fsFire, {AnchorEdgeSide::RIGHT}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);

    motorStateMachine.addStateRect(fsInit);
    motorStateMachine.addStateRect(fsSafe);
    motorStateMachine.addStateRect(fsTest);
    motorStateMachine.addStateRect(fsAbort);
    motorStateMachine.addStateRect(fsError);
    motorStateMachine.addStateRect(fsUnsafe);
    motorStateMachine.addStateRect(fsIgnite);
    motorStateMachine.addStateRect(fsFire);
    motorStateMachine.addLabel(fsTestToErrorLabel);
    motorStateMachine.addLabel(fsIgniteToSafeLabel);
    motorStateMachine.addLabel(fsFireToSafeLabel);
    motorStateMachine.addArrow(fsInitToSafe);
    motorStateMachine.addArrow(fsInitToError);
    motorStateMachine.addArrow(fsSafeToTest);
    motorStateMachine.addArrow(fsSafeToUnsafe);
    motorStateMachine.addArrow(fsSafeToError);
    motorStateMachine.addArrow(fsSafeToAbort);
    motorStateMachine.addArrow(fsTestToSafe);
    motorStateMachine.addArrow(fsTestToError);
    motorStateMachine.addArrow(fsAbortToSafe);
    motorStateMachine.addArrow(fsErrorToSafe);
    motorStateMachine.addArrow(fsErrorToAbort);
    motorStateMachine.addArrow(fsUnsafeToSafe);
    motorStateMachine.addArrow(fsUnsafeToIgnite);
    motorStateMachine.addArrow(fsUnsafeToError);
    motorStateMachine.addArrow(fsUnsafeToAbort);
    motorStateMachine.addArrow(fsIgniteToSafe);
    motorStateMachine.addArrow(fsIgniteToAbort);
    motorStateMachine.addArrow(fsIgniteToFire);
    motorStateMachine.addArrow(fsFireToSafe);
    motorStateMachine.addArrow(fsFireToAbort);
}

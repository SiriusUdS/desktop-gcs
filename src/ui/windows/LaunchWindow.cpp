#include "LaunchWindow.h"

#include "ImGuiConfig.h"

#include <algorithm>
#include <imgui.h>

using StateRect = StateMachineRenderer::StateRect;
using Arrow = StateMachineRenderer::Arrow;
using ArrowPathType = StateMachineRenderer::ArrowPathType;
using AnchorEdgeSide = StateMachineRenderer::AnchorEdgeSide;
using ArrowheadDir = StateMachineRenderer::ArrowheadDir;

LaunchWindow::LaunchWindow() {
    ImVec2 rectSize{240, 60};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect fsInit{.position{-450, 0}, .size{rectSize}, .active{false}, .label{"INIT"}};
    StateRect fsSafe{.position{0, 0}, .size{rectSize}, .active{true}, .label{"SAFE"}};
    StateRect fsTest{.position{0, -210}, .size{rectSize}, .active{false}, .label{"TEST"}};
    StateRect fsAbort{.position{450, -210}, .size{rectSize}, .active{false}, .label{"ABORT"}};
    StateRect fsError{.position{-225, 210}, .size{rectSize}, .active{false}, .label{"ERROR"}};
    StateRect fsUnsafe{.position{225, 210}, .size{rectSize}, .active{false}, .label{"UNSAFE"}};
    StateRect fsIgnite{.position{225, 420}, .size{rectSize}, .active{false}, .label{"IGNITE"}};

    Arrow fsInitToSafe = StateMachineRenderer::createArrow(fsInit, {AnchorEdgeSide::RIGHT}, fsSafe, {AnchorEdgeSide::LEFT});
    fsInitToSafe.arrowhead = true;
    fsInitToSafe.arrowheadDirection = ArrowheadDir::RIGHT;

    Arrow fsInitToError = StateMachineRenderer::createArrow(fsInit, {AnchorEdgeSide::BOTTOM}, fsError, {AnchorEdgeSide::TOP, 0.25f});
    fsInitToError.arrowhead = true;
    fsInitToError.arrowheadDirection = ArrowheadDir::DOWN;
    fsInitToError.label = "[Init Failed]";

    Arrow fsErrorToSafe = StateMachineRenderer::createArrow(fsError, {AnchorEdgeSide::TOP, 0.75f}, fsSafe, {AnchorEdgeSide::BOTTOM, 0.75f});
    fsErrorToSafe.arrowhead = true;
    fsErrorToSafe.arrowheadDirection = ArrowheadDir::UP;
    fsErrorToSafe.label = "SAFE";

    Arrow fsErrorToAbort =
      StateMachineRenderer::createArrow(fsError, {AnchorEdgeSide::BOTTOM}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::HORIZONTAL, 500.0f);
    fsErrorToAbort.arrowhead = true;
    fsErrorToAbort.arrowheadDirection = ArrowheadDir::UP;

    Arrow fsSafeToTest = StateMachineRenderer::createArrow(fsSafe, {AnchorEdgeSide::TOP}, fsTest, {AnchorEdgeSide::BOTTOM});
    fsSafeToTest.arrowhead = true;
    fsSafeToTest.arrowheadDirection = ArrowheadDir::UP;
    fsSafeToTest.label = "TEST";

    Arrow fsSafeToUnsafe = StateMachineRenderer::createArrow(fsSafe, {AnchorEdgeSide::BOTTOM, 0.25f}, fsUnsafe, {AnchorEdgeSide::TOP, 0.25f});
    fsSafeToUnsafe.arrowhead = true;
    fsSafeToUnsafe.arrowheadDirection = ArrowheadDir::DOWN;
    fsSafeToUnsafe.label = "UNSAFE";

    Arrow fsSafeToAbort = StateMachineRenderer::createArrow(fsSafe, {AnchorEdgeSide::TOP, 0.75f}, fsAbort, {AnchorEdgeSide::BOTTOM});
    fsSafeToAbort.arrowhead = true;
    fsSafeToAbort.arrowheadDirection = ArrowheadDir::UP;

    Arrow fsTestToSafe = StateMachineRenderer::createArrow(fsTest, {AnchorEdgeSide::BOTTOM, 0.85f}, fsSafe, {AnchorEdgeSide::TOP, 0.15f});
    fsTestToSafe.arrowhead = true;
    fsTestToSafe.arrowheadDirection = ArrowheadDir::DOWN;
    fsTestToSafe.label = "SAFE";

    Arrow fsUnsafeToSafe =
      StateMachineRenderer::createArrow(fsUnsafe, {AnchorEdgeSide::TOP}, fsSafe, {AnchorEdgeSide::RIGHT}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToSafe.arrowhead = true;
    fsUnsafeToSafe.arrowheadDirection = ArrowheadDir::LEFT;
    fsUnsafeToSafe.label = "SAFE";

    Arrow fsUnsafeToIgnite = StateMachineRenderer::createArrow(fsUnsafe, {AnchorEdgeSide::BOTTOM}, fsIgnite, {AnchorEdgeSide::TOP});
    fsUnsafeToIgnite.arrowhead = true;
    fsUnsafeToIgnite.arrowheadDirection = ArrowheadDir::DOWN;
    fsUnsafeToIgnite.label = "IGNITE";

    Arrow fsUnsafeToAbort =
      StateMachineRenderer::createArrow(fsUnsafe, {AnchorEdgeSide::RIGHT}, fsAbort, {AnchorEdgeSide::BOTTOM}, ArrowPathType::ORTHOGONAL);
    fsUnsafeToAbort.arrowhead = true;
    fsUnsafeToAbort.arrowheadDirection = ArrowheadDir::UP;
    fsUnsafeToAbort.label = "ABORT";

    fsStateMachine.addStateRect(fsInit);
    fsStateMachine.addStateRect(fsSafe);
    fsStateMachine.addStateRect(fsTest);
    fsStateMachine.addStateRect(fsAbort);
    fsStateMachine.addStateRect(fsError);
    fsStateMachine.addStateRect(fsUnsafe);
    fsStateMachine.addStateRect(fsIgnite);
    fsStateMachine.addArrow(fsInitToSafe);
    fsStateMachine.addArrow(fsInitToError);
    fsStateMachine.addArrow(fsErrorToSafe);
    fsStateMachine.addArrow(fsErrorToAbort);
    fsStateMachine.addArrow(fsSafeToTest);
    fsStateMachine.addArrow(fsSafeToUnsafe);
    fsStateMachine.addArrow(fsSafeToAbort);
    fsStateMachine.addArrow(fsTestToSafe);
    fsStateMachine.addArrow(fsUnsafeToSafe);
    fsStateMachine.addArrow(fsUnsafeToIgnite);
    fsStateMachine.addArrow(fsUnsafeToAbort);
}

void LaunchWindow::render() {
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    fsStateMachine.render(windowSize);
}

const char* LaunchWindow::name() const {
    return "Launch";
}

const char* LaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

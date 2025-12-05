#include "LaunchWindow.h"

#include "ImGuiConfig.h"

#include <algorithm>
#include <imgui.h>

using StateRect = StateMachineRenderer::StateRect;
using Arrow = StateMachineRenderer::Arrow;
using ArrowType = StateMachineRenderer::ArrowType;
using AnchorPointDir = StateMachineRenderer::AnchorPointDir;
using ArrowheadDir = StateMachineRenderer::ArrowheadDir;

LaunchWindow::LaunchWindow() {
    ImVec2 rectSize{160, 60};
    ImVec2 halfRectSize{rectSize.x / 2.0f, rectSize.y / 2.0f};

    StateRect fsInit{.position{-300, 0}, .size{rectSize}, .active{false}, .label{"INIT"}};
    StateRect fsSafe{.position{0, 0}, .size{rectSize}, .active{true}, .label{"SAFE"}};
    StateRect fsError{.position{-150, 140}, .size{rectSize}, .active{false}, .label{"ERROR"}};

    Arrow fsInitToSafe = StateMachineRenderer::createArrow(fsInit, AnchorPointDir::RIGHT, fsSafe, AnchorPointDir::LEFT, ArrowType::HORIZONTAL);
    fsInitToSafe.arrowhead = true;
    fsInitToSafe.arrowheadDirection = ArrowheadDir::RIGHT;

    Arrow fsInitToError = StateMachineRenderer::createArrow(fsInit, AnchorPointDir::BOTTOM, fsError, AnchorPointDir::TOP, ArrowType::HORIZONTAL);
    fsInitToError.arrowhead = true;
    fsInitToError.arrowheadDirection = ArrowheadDir::DOWN;
    fsInitToError.label = "[Init Failed]";

    Arrow fsSafeToError = StateMachineRenderer::createArrow(fsSafe, AnchorPointDir::BOTTOM, fsError, AnchorPointDir::TOP, ArrowType::HORIZONTAL);
    fsSafeToError.arrowhead = true;
    fsSafeToError.arrowheadDirection = ArrowheadDir::DOWN;

    fsStateMachine.addStateRect(fsInit);
    fsStateMachine.addStateRect(fsSafe);
    fsStateMachine.addStateRect(fsError);
    fsStateMachine.addArrow(fsInitToSafe);
    fsStateMachine.addArrow(fsInitToError);
    fsStateMachine.addArrow(fsSafeToError);
}

void LaunchWindow::render() {
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    fsStateMachine.render(windowSize, true);
}

const char* LaunchWindow::name() const {
    return "Launch";
}

const char* LaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

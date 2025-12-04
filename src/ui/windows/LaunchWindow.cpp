#include "LaunchWindow.h"

#include "ImGuiConfig.h"

#include <algorithm>
#include <imgui.h>

LaunchWindow::LaunchWindow() {
    StateMachineRenderer::StateRect rect1{.position{100, 0}, .size{200, 80}, .active = true, .label = "SAFE"};
    StateMachineRenderer::StateRect rect2{.position{100, 200}, .size{200, 80}, .active = false, .label = "UNSAFE"};
    StateMachineRenderer::StateRect rect3{.position{-200, 0}, .size{200, 80}, .active = false, .label = "INIT"};
    StateMachineRenderer::Arrow arrow1{.points = {{100, 40}, {100, 160}},
                                       .arrowhead = true,
                                       .arrowheadDirection = StateMachineRenderer::ArrowheadDirection::DOWN};
    StateMachineRenderer::Arrow arrow2{.points = {{-100, 0}, {-66, 0}, {-66, 33}, {-33, 33}, {-33, 0}, {0, 0}},
                                       .arrowhead = true,
                                       .arrowheadDirection = StateMachineRenderer::ArrowheadDirection::RIGHT};

    stateMachineRenderer.addStateRect(rect1);
    stateMachineRenderer.addStateRect(rect2);
    stateMachineRenderer.addStateRect(rect3);
    stateMachineRenderer.addArrow(arrow1);
    stateMachineRenderer.addArrow(arrow2);
}

void LaunchWindow::render() {
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 size = {windowSize.x, 2000};
    stateMachineRenderer.render(size, true);

    ImGui::Text("this is a test");
}

const char* LaunchWindow::name() const {
    return "Launch";
}

const char* LaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

#include "PrelaunchWindow.h"

#include "ImGuiConfig.h"

#include <imgui.h>

const char* PrelaunchWindow::name() const {
    return "Prelaunch";
}

const char* PrelaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void PrelaunchWindow::renderImpl() {
    ImGui::Text("TODO: Note ADC loadcell post NOS");
    ImGui::Button("Test igniter continuity");
}

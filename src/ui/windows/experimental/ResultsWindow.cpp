#include "ResultsWindow.h"

#include "ImGuiConfig.h"

#include <imgui.h>

const char* const ResultsWindow::name = "Results";

const char* ResultsWindow::getName() const {
    return name;
}

const char* ResultsWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void ResultsWindow::renderImpl() {
    ImGui::Text("TODO: SD Card program");
}

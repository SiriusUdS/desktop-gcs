#include "ResultsWindow.h"

#include <imgui.h>

const char* const ResultsWindow::name = "Results";

const char* ResultsWindow::getName() const {
    return name;
}

void ResultsWindow::renderImpl() {
    ImGui::Text("TODO: SD Card program");
}

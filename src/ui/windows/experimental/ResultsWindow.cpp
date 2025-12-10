#include "ResultsWindow.h"

#include "ImGuiConfig.h"

const char* ResultsWindow::name() const {
    return "Results";
}

const char* ResultsWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void ResultsWindow::renderImpl() {
}

#include "FillWindow.h"

#include "ImGuiConfig.h"

const char* FillWindow::name() const {
    return "Fill";
}

const char* FillWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void FillWindow::renderImpl() {
}

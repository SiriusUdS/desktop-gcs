#include "PrefillWindow.h"

#include "ImGuiConfig.h"

#include <imgui.h>

const char* PrefillWindow::name() const {
    return "Prefill";
}

const char* PrefillWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void PrefillWindow::renderImpl() {
    if (ImGui::Button("Test NOS Valve")) {
        sensorTestSequencer.testNOSValve();
    }
}

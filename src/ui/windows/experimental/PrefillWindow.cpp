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
    ImGui::SeparatorText("Tests");

    ImGui::BeginDisabled(sensorTestSequencer.isBusy());

    if (ImGui::Button("Test NOS Valve")) {
        sensorTestSequencer.testNOSValve();
    }

    if (ImGui::Button("Test IPA Valve")) {
        sensorTestSequencer.testIPAValve();
    }

    if (ImGui::Button("Test Fill Valve")) {
        sensorTestSequencer.testFillValve();
    }

    if (ImGui::Button("Test Dump Valve")) {
        sensorTestSequencer.testDumpValve();
    }

    ImGui::EndDisabled();
}

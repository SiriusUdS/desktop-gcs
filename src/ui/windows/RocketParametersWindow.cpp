#include "RocketParametersWindow.h"

#include "RocketParams.h"

#include <imgui.h>

void RocketParametersWindow::render() {
    ImGui::Text("Changing the rocket parameters below will update related calculations in other windows.");
    ImGui::InputFloat("Tank volume (m^3)", &RocketParams::NOSTankVolume_m3.currentValue, 0, 0, "%.6f");
    ImGui::SameLine();
    if (ImGui::Button("Restore Default")) {
        RocketParams::NOSTankVolume_m3.currentValue = RocketParams::NOSTankVolume_m3.defaultValue;
    }
}

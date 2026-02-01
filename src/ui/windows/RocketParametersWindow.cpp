#include "RocketParametersWindow.h"

#include "IniParams.h"

#include <imgui.h>

RocketParametersWindow::RocketParametersWindow() : NOSTankVolume_m3(IniParams::NOSTankVolume_m3.currentValue) {
}

const char* RocketParametersWindow::getName() const {
    return "Rocket Parameters";
}

void RocketParametersWindow::renderImpl() {
    ImGui::Text("Changing the rocket parameters below will update related calculations in other windows.");
    if (ImGui::InputFloat("Tank volume (m^3)", &NOSTankVolume_m3, 0, 0, "%.6f")) {
        IniParams::NOSTankVolume_m3.currentValue = NOSTankVolume_m3;
    }
    ImGui::SameLine();
    if (ImGui::Button("Restore Default")) {
        IniParams::NOSTankVolume_m3.reset();
        NOSTankVolume_m3 = IniParams::NOSTankVolume_m3.currentValue;
    }
}

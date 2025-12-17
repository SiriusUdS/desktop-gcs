#include "RocketParametersWindow.h"

#include "ImGuiConfig.h"
#include "Params.h"

#include <imgui.h>

const char* RocketParametersWindow::name() const {
    return "Rocket Parameters";
}

const char* RocketParametersWindow::dockspace() const {
    return ImGuiConfig::Dockspace::LOGGING;
}

void RocketParametersWindow::renderImpl() {
    ImGui::Text("Changing the rocket parameters below will update related calculations in other windows.");
    ImGui::InputFloat("Tank volume (m^3)", &Params::NOSTankVolume_m3.currentValue, 0, 0, "%.6f");
    ImGui::SameLine();
    if (ImGui::Button("Restore Default")) {
        Params::NOSTankVolume_m3.currentValue = Params::NOSTankVolume_m3.defaultValue;
    }
}

#include "TankMassCalculatorWindow.h"

#include "FontAwesome.h"
#include "RocketParams.h"
#include "TankMass.h"

#include <cmath>
#include <imgui.h>

namespace TankMassCalculatorWindow {
double tankTemperature_C{};
double tankPressure_psi{};
} // namespace TankMassCalculatorWindow

void TankMassCalculatorWindow::render() {
    ImGui::Text("Tank Volume (m^3): %f", RocketParams::NOSTankVolume_m3);
    ImGui::SameLine();

    const ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
    ImGui::Button(ICON_FA_QUESTION);
    ImGui::PopStyleColor(2);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The tank volume can be modified applicationwide in the \"Rocket Parameters\" window.");
    }

    ImGui::InputDouble("Temperature (C)", &tankTemperature_C);
    ImGui::InputDouble("Pressure (psi)", &tankPressure_psi);

    ImGui::Text("Tank Mass (lb): ");
    ImGui::SameLine();

    const float tankMass_lb = TankMass::getNOSTankMass_lb(tankTemperature_C, tankPressure_psi);

    if (std::isnan(tankMass_lb)) {
        ImGui::Text("Invalid");
    } else if (std::isinf(tankMass_lb)) {
        ImGui::Text("Infinity");
    } else {
        ImGui::Text("%f", tankMass_lb);
    }
}

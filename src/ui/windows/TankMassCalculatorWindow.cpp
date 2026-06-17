#include "TankMassCalculatorWindow.h"

#include "FontAwesome.h"
#include "IniParams.h"
#include "TankMass.h"
#include "units.h"

#include <imgui.h>

const char* TankMassCalculatorWindow::getName() const {
    return "Tank Mass Calculator";
}

void TankMassCalculatorWindow::renderImpl() {
    ImGui::Text("Tank Volume (m^3): %f", IniParams::NOSTankVolume_m3.currentValue.load());
    ImGui::SameLine();

    const ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
    ImGui::Button(ICON_FA_QUESTION);
    ImGui::PopStyleColor(2);
        
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("The tank volume can be modified applicationwide in the \"Rocket Parameters\" window.");
    }

    ImGui::InputDouble(Units::as_label(Units::DEFAULT_TEMPERATURE_UNIT), &tankTemperature);
    ImGui::InputDouble(Units::as_label(Units::DEFAULT_PRESSURE_UNIT), &tankPressure);

    ImGui::Text("Tank Mass (lb): ");
    ImGui::SameLine();

    float tankTemperature_C = Units::convert(tankTemperature, Units::DEFAULT_TEMPERATURE_UNIT, Units::TemperatureUnit::Celcius);
    float tankPressure_psi = Units::convert(tankPressure, Units::DEFAULT_PRESSURE_UNIT, Units::PressureUnit::Psi);
    const float tankMass_lb = TankMass::getNOSTankMass_lb(tankTemperature_C, tankPressure_psi);
    const float tankMass = Units::convert(tankMass_lb, Units::WeightUnit::Pounds, Units::DEFAULT_WEIGHT_UNIT);

    if (std::isnan(tankMass_lb)) {
        ImGui::Text("Invalid");
    } else if (std::isinf(tankMass_lb)) {
        ImGui::Text("Infinity");
    } else {
        ImGui::Text("%f", tankMass_lb);
    }
}

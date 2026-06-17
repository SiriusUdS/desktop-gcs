#include "PrelaunchWindow.h"

#include "AppState.h"
#include "GSDataCenter.h"
#include "IniParams.h"
#include "LaunchWindow.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "RecentDataSelector.h"
#include "units.h"

#include <imgui.h>
#include <implot.h>

const char* const PrelaunchWindow::name = "Prelaunch";

PrelaunchWindow::PrelaunchWindow()
    : postNOSTankLoadCellState(AppState::TankLoadCell::postNOSADCValue, "Post NOS"),
      tankLoadCellADCPlotLine(GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(),
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue), std::make_unique<RecentDataSelector>(60000, Units::TimeUnit::Milliseconds)),
      tankLoadCellPlotLine(GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell Weight", ThemedColors::PlotLine::red), std::make_unique<RecentDataSelector>(60000, Units::TimeUnit::Milliseconds)) {
}

const char* PrelaunchWindow::getName() const {
    return name;
}

void PrelaunchWindow::renderImpl() {
    constexpr Units::TimeUnit TIME_UNIT = Units::TimeUnit::Seconds;
    constexpr Units::WeightUnit WEIGHT_UNIT = Units::DEFAULT_WEIGHT_UNIT;
    constexpr Units::PressureUnit PRESSURE_UNIT = Units::DEFAULT_PRESSURE_UNIT;
    constexpr Units::TemperatureUnit TEMPERATURE_UNIT = Units::DEFAULT_TEMPERATURE_UNIT;
    constexpr Units::Unit ADC_UNIT = Units::QuantityUnit::Scalar;

    ImGui::SeparatorText("Calibration");
    ImGui::Text("Tank Load Cell ADC Value");
    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 6, ImGuiTableFlags_SizingFixedFit)) {
        const SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.tank();
        const float avgAdcValue = tankLoadCellData.getAdcPlotData().recentAverageValue();

        postNOSTankLoadCellState.renderAsRow(avgAdcValue);

        ImGui::EndTable();
    }

    const ImVec2 plotSize = {-1.0f, 500.0f};
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Load Cell (ADC)", plotSize, ImPlotFlags_NoInputs)) {
        constexpr ImAxis adcValueAxis = ImAxis_Y1;
        constexpr ImAxis weightAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, Units::as_label(TIME_UNIT));
        ImPlot::SetupAxis(adcValueAxis, Units::as_label(ADC_UNIT));
        ImPlot::SetupAxis(weightAxis, Units::as_label(WEIGHT_UNIT));

        ImPlot::SetAxis(adcValueAxis);
        tankLoadCellADCPlotLine.plot(TIME_UNIT, ADC_UNIT, IniParams::compressPlots.currentValue, true);

        ImPlot::SetAxis(weightAxis);
        tankLoadCellPlotLine.plot(TIME_UNIT, WEIGHT_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Igniter");
    ImGui::Button("Test igniter continuity");

    ImGui::SeparatorText("Switch to \"Launch\"");
    const bool allowConfirm = postNOSTankLoadCellState.state.saved;
    ImGui::BeginDisabled(!allowConfirm);
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(LaunchWindow::name);
    }
    if (!allowConfirm && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("All calibration values need to be saved before proceeding to the \"Launch\" window.");
    }
    ImGui::EndDisabled();
}

#include "PrefillWindow.h"

#include "AppState.h"
#include "FillWindow.h"
#include "GSDataCenter.h"
#include "IniParams.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "RecentDataSelector.h"
#include "units.h"

#include <imgui.h>
#include <implot.h>
#include <iostream>

const char* const PrefillWindow::name = "Prefill";

PrefillWindow::PrefillWindow()
    : prewrapTankLoadCellState{AppState::TankLoadCell::prewrapADCValue, "Prewrap"},
      postwrapTankLoadCellState{AppState::TankLoadCell::postwrapADCValue, "Postwrap"},
      postIPATankLoadCellState{AppState::TankLoadCell::postIPAADCValue, "Post IPA"},
      tankLoadCellADCPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(),
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue), std::make_unique<RecentDataSelector>(60000, Units::TimeUnit::Milliseconds)},
      tankLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell Weight", ThemedColors::PlotLine::red), std::make_unique<RecentDataSelector>(60000, Units::TimeUnit::Milliseconds)} {
}

const char* PrefillWindow::getName() const {
    return name;
}

void PrefillWindow::renderImpl() {
    constexpr Units::TimeUnit TIME_UNIT = Units::TimeUnit::Seconds;
    constexpr Units::WeightUnit WEIGHT_UNIT = Units::DEFAULT_WEIGHT_UNIT;
    constexpr Units::PressureUnit PRESSURE_UNIT = Units::DEFAULT_PRESSURE_UNIT;
    constexpr Units::TemperatureUnit TEMPERATURE_UNIT = Units::DEFAULT_TEMPERATURE_UNIT;
    constexpr Units::Unit ADC_UNIT = Units::QuantityUnit::Scalar;
    
    ImGui::SeparatorText("Calibration GS");
    ImGui::Text("Tank Load Cell ADC Values");

    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 6, ImGuiTableFlags_SizingFixedFit)) {
        const SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.tank();
        const float avgAdcValue = tankLoadCellData.getAdcPlotData().recentAverageValue();

        prewrapTankLoadCellState.renderAsRow(avgAdcValue);
        postwrapTankLoadCellState.renderAsRow(avgAdcValue);
        postIPATankLoadCellState.renderAsRow(avgAdcValue);

        ImGui::EndTable();
    }

    const ImVec2 plotSize = {-1.0f, 500.0f};
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Load Cell", plotSize, ImPlotFlags_NoInputs)) {
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

    ImGui::SeparatorText("Switch to \"Fill\"");
    const bool allowConfirm = prewrapTankLoadCellState.state.saved && postwrapTankLoadCellState.state.saved && postIPATankLoadCellState.state.saved;
    ImGui::BeginDisabled(!allowConfirm);
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(FillWindow::name);
    }
    ImGui::EndDisabled();
    if (!allowConfirm && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("All calibration values need to be saved before proceeding to the \"Fill\" window.");
    }

    ImGui::SeparatorText("Ematch Info");
    renderIndicator("Energized", GSDataCenter::latestFcuExtendedSystemState.load().ematch_info.status.energised > 0);
    renderIndicator("Detected", GSDataCenter::latestFcuExtendedSystemState.load().ematch_info.status.detected > 0);
    if (tankLoadCellPlotLine.getData().getValues().raw().size() > 0) {
        ImGui::Text("%f", tankLoadCellPlotLine.getData().getValues().raw().back());
    }
}

void PrefillWindow::renderIndicator(const char* name, bool high, ImColor color_low, ImColor color_high, const char* label_low, const char* label_high) {
    ImColor color = high ? color_high : color_low;
    ImGui::PushStyleColor(ImGuiCol_Button, color.Value);         // Idle state
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color.Value);  // Hovered state
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color.Value);   // Clicked state

    std::string status = std::string(high ? label_high : label_low) + "##" + name;
    ImGui::Button(status.c_str());

    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    ImGui::SameLine();
    ImGui::Text(name);

    ImGui::PopStyleColor(3);
}
#include "PrelaunchWindow.h"

#include "AppState.h"
#include "ConfigParams.h"
#include "GSDataCenter.h"
#include "LaunchWindow.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"

#include <imgui.h>
#include <implot.h>

const char* const PrelaunchWindow::name = "Prelaunch";

PrelaunchWindow::PrelaunchWindow()
    : postNOSTankLoadCellState(AppState::TankLoadCell::postNOSADCValue, "Post NOS"),
      tankLoadCellADCPlotLine(GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(),
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue)),
      tankLoadCellPlotLine(GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell Weight", ThemedColors::PlotLine::red)) {
}

const char* PrelaunchWindow::getName() const {
    return name;
}

void PrelaunchWindow::renderImpl() {
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

        ImPlot::SetupAxis(ImAxis_X1, "Timestamp (ms)");
        ImPlot::SetupAxis(adcValueAxis, "ADC Value");
        ImPlot::SetupAxis(weightAxis, "Weight (lb)");

        ImPlot::SetAxis(adcValueAxis);
        tankLoadCellADCPlotLine.plot(ConfigParams::compressPlots.currentValue);

        ImPlot::SetAxis(weightAxis);
        tankLoadCellPlotLine.plot(ConfigParams::compressPlots.currentValue);

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

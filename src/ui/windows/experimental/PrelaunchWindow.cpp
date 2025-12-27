#include "PrelaunchWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"
#include "UiState.h"

#include <imgui.h>

PrelaunchWindow::PrelaunchWindow() : postNOSTankLoadCellState(UiState::TankLoadCell::postNOSADCValue, "Post NOS") {
}

const char* PrelaunchWindow::name() const {
    return "Prelaunch";
}

const char* PrelaunchWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
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

    ImGui::SeparatorText("Igniter");
    ImGui::Button("Test igniter continuity");
}

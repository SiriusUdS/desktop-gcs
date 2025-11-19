#include "TankMassWindow.h"

#include "GSDataCenter.h"
#include "PlotColors.h"
#include "RecentPlotData.h"
#include "SensorPlotData.h"
#include "VaporPressure.h"

#include <imgui.h>
#include <implot.h>
#include <string>

namespace TankMassWindow {
std::string getRecentPlotTitle(std::string title, size_t seconds);

constexpr size_t RECENT_TIME_WINDOW_SEC = 60;
const std::string TANK_PRESSURE_PLOT_TITLE = getRecentPlotTitle("Tank Pressure", RECENT_TIME_WINDOW_SEC);
const std::string TANK_TEMPERATURE_PLOT_TITLE = getRecentPlotTitle("Tank Temperature", RECENT_TIME_WINDOW_SEC);
const std::string ENGINE_THRUST_PLOT_TITLE = getRecentPlotTitle("Engine Thrust", RECENT_TIME_WINDOW_SEC);
const std::string TANK_MASS_PLOT_TITLE = getRecentPlotTitle("Tank Mass", RECENT_TIME_WINDOW_SEC);

// TODO: Replace these indexes with constants
RecentPlotData recentMotorPressureSensor1{GSDataCenter::PressureSensor_Motor_PlotData[0].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentMotorPressureSensor2{GSDataCenter::PressureSensor_Motor_PlotData[1].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentFillPressureSensor1{GSDataCenter::PressureSensor_FillingStation_PlotData[0].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentFillPressureSensor2{GSDataCenter::PressureSensor_FillingStation_PlotData[1].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentTankTemperature{GSDataCenter::Thermistor_Motor_PlotData[2].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentEngineThrust{GSDataCenter::LoadCell_FillingStation_PlotData[0].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentTankMass{GSDataCenter::NOSTankMass_PlotData, RECENT_TIME_WINDOW_SEC * 1000};
RecentPlotData recentTankLoadCell{GSDataCenter::LoadCell_FillingStation_PlotData[1].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
} // namespace TankMassWindow

void TankMassWindow::init() {
    recentFillPressureSensor1.setColor(PlotColors::GREEN);
    recentFillPressureSensor2.setColor(PlotColors::YELLOW);
    recentTankTemperature.setColor(PlotColors::BLUE);
}

void TankMassWindow::render() {
    constexpr double TEMP_VALUE = 1.0;

    constexpr int PLOT_ROWS = 2;

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float paddingY = ImGui::GetStyle().CellPadding.y * 3.0f;
    const float plotRowHeight = (avail.y / PLOT_ROWS) - paddingY;

    if (ImGui::BeginTable("Tank Mass", PLOT_ROWS)) {
        ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Plots");
        ImGui::TableNextRow();

        const double vaporPressure_psi = VaporPressure::vaporPressureNOS_psi(GSDataCenter::tankTemperature_C);
        const std::string phaseStr = GSDataCenter::tankPressure_psi > vaporPressure_psi ? "Liquid" : "Gas";

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Temp Tank (C): %f", GSDataCenter::tankTemperature_C);
        ImGui::Text("PT Tank (psi): %f", GSDataCenter::tankPressure_psi);
        ImGui::Text("LC Tank (lb): %f", GSDataCenter::tankLoadCell_lb);
        ImGui::Text("Phase: %s", phaseStr.c_str());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginTable("Plots", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(TANK_PRESSURE_PLOT_TITLE.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Pressure (psi)");
                recentMotorPressureSensor1.plot(false);
                recentMotorPressureSensor2.plot(false);
                recentFillPressureSensor1.plot(false);
                recentFillPressureSensor2.plot(false);
                ImPlot::EndPlot();
            }
            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(TANK_TEMPERATURE_PLOT_TITLE.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Temperature (C)");
                recentTankTemperature.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(ENGINE_THRUST_PLOT_TITLE.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Thrust (lb)");
                recentEngineThrust.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(TANK_MASS_PLOT_TITLE.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Mass (lb)");
                recentTankMass.plot(false);
                recentTankLoadCell.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::EndTable();
        }

        ImGui::EndTable();
    }

    // TODO: THIS IS A TEST TO REMOVE LATER
    static float tempY = 0;
    static float pressureY = 0;
    ImGui::InputFloat("Test Temp", &tempY);
    ImGui::InputFloat("Test Pressure", &pressureY);
    if (ImGui::Button("Add temp")) {
        static float thermistorX = 0;
        GSDataCenter::Thermistor_Motor_PlotData[2].addData(1, tempY, thermistorX);
        thermistorX += 10000;
    }
    if (ImGui::Button("Add pressure")) {
        static float pressureX = 0;
        GSDataCenter::PressureSensor_Motor_PlotData[0].addData(1, pressureY, pressureX);
        pressureX += 10000;
    }
}

std::string TankMassWindow::getRecentPlotTitle(std::string title, size_t seconds) {
    return title + " (last " + std::to_string(seconds) + " seconds)";
}

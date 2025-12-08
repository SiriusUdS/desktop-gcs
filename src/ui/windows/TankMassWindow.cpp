#include "TankMassWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"
#include "VaporPressure.h"

#include <imgui.h>
#include <implot.h>
#include <string>

void TankMassWindow::init() {
    tankPressurePlotTitle = getRecentPlotTitle("Tank Pressure", RECENT_TIME_WINDOW_MS / 1000);
    tankTemperaturePlotTitle = getRecentPlotTitle("Tank Temperature", RECENT_TIME_WINDOW_MS / 1000);
    engineThrustPlotTitle = getRecentPlotTitle("Engine Thrust", RECENT_TIME_WINDOW_MS / 1000);
    tankMassPlotTitle = getRecentPlotTitle("Tank Mass", RECENT_TIME_WINDOW_MS / 1000);
}

void TankMassWindow::renderImpl() {
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
        ImGui::Text("Temp Tank (C): %f", GSDataCenter::tankTemperature_C.load());
        ImGui::Text("PT Tank (psi): %f", GSDataCenter::tankPressure_psi.load());
        ImGui::Text("LC Tank (lb): %f", GSDataCenter::tankLoadCell_lb.load());
        ImGui::Text("Phase: %s", phaseStr.c_str());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginTable("Plots", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankPressurePlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Pressure (psi)");
                recentMotorPressureSensor1.plot(false);
                recentMotorPressureSensor2.plot(false);
                recentFillPressureSensor1.plot(false);
                recentFillPressureSensor2.plot(false);
                ImPlot::EndPlot();
            }
            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankTemperaturePlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Temperature (C)");
                recentTankTemperature.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(engineThrustPlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Thrust (lb)");
                recentEngineThrust.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankMassPlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes("Timestamp (ms)", "Mass (lb)");
                recentTankMass.plot(false);
                recentTankLoadCell.plot(false);
                ImPlot::EndPlot();
            }

            ImGui::EndTable();
        }

        ImGui::EndTable();
    }
}

const char* TankMassWindow::name() const {
    return "Tank Mass";
}

const char* TankMassWindow::dockspace() const {
    return ImGuiConfig::Dockspace::PLOT;
}

std::string TankMassWindow::getRecentPlotTitle(std::string title, size_t seconds) {
    return title + " (last " + std::to_string(seconds) + " seconds)";
}

#include "TankMassWindow.h"

#include "GSDataCenter.h"
#include "SensorPlotData.h"
#include "StringUtils.h"
#include "ThemedColors.h"
#include "VaporPressure.h"
#include "units.h"

#include <imgui.h>
#include <implot.h>
#include <string>

TankMassWindow::TankMassWindow()
    : recentMotorPressureSensor1{GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData(),
                                 PlotStyle("Pressure Sensor 1 (Motor)", ThemedColors::PlotLine::blue),
                                 RECENT_TIME_WINDOW_MS},
      recentMotorPressureSensor2{GSDataCenter::PressureSensor_Motor_PlotData.p2().getValuePlotData(),
                                 PlotStyle("Pressure Sensor 2 (Motor)", ThemedColors::PlotLine::red),
                                 RECENT_TIME_WINDOW_MS},
      recentFillPressureSensor1{GSDataCenter::PressureSensor_FillingStation_PlotData.p1().getValuePlotData(),
                                PlotStyle("Pressure Sensor 1 (Fill)", ThemedColors::PlotLine::green),
                                RECENT_TIME_WINDOW_MS},
      recentFillPressureSensor2{GSDataCenter::PressureSensor_FillingStation_PlotData.p2().getValuePlotData(),
                                PlotStyle("Pressure Sensor 2 (Fill)", ThemedColors::PlotLine::yellow),
                                RECENT_TIME_WINDOW_MS},
      recentTankTemperature{GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData(),
                            PlotStyle("Tank Thermistor", ThemedColors::PlotLine::blue),
                            RECENT_TIME_WINDOW_MS},
      recentEngineThrust{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                         PlotStyle("Motor Load Cell", ThemedColors::PlotLine::blue),
                         RECENT_TIME_WINDOW_MS},
      recentTankMass{GSDataCenter::NOSTankMass_PlotData, PlotStyle("NOS Tank Mass", ThemedColors::PlotLine::blue), RECENT_TIME_WINDOW_MS},
      recentTankLoadCell{GSDataCenter::LoadCell_FillingStation_PlotData.tank().getValuePlotData(),
                         PlotStyle("Tank Load Cell", ThemedColors::PlotLine::red),
                         RECENT_TIME_WINDOW_MS} {
}

void TankMassWindow::init() {
    tankPressurePlotTitle = StringUtils::getRecentPlotLineTitle("Tank Pressure", RECENT_TIME_WINDOW_MS / 1000);
    tankTemperaturePlotTitle = StringUtils::getRecentPlotLineTitle("Tank Temperature", RECENT_TIME_WINDOW_MS / 1000);
    engineThrustPlotTitle = StringUtils::getRecentPlotLineTitle("Engine Thrust", RECENT_TIME_WINDOW_MS / 1000);
    tankMassPlotTitle = StringUtils::getRecentPlotLineTitle("Tank Mass", RECENT_TIME_WINDOW_MS / 1000);
}

const char* TankMassWindow::getName() const {
    return "Tank Mass";
}

void TankMassWindow::renderImpl() {
    constexpr double TEMP_VALUE = 1.0;

    constexpr int PLOT_ROWS = 2;

    constexpr Units::TimeUnit TIME_UNIT = Units::TimeUnit::Seconds;

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const float paddingY = ImGui::GetStyle().CellPadding.y * 3.0f;
    const float plotRowHeight = (avail.y / PLOT_ROWS) - paddingY;

    if (ImGui::BeginTable("Tank Mass", PLOT_ROWS)) {
        ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Plots");
        ImGui::TableNextRow();

        float tankTemperature_C = GSDataCenter::Thermistor_Motor_PlotData.tank().getSize()
                                    ? GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData().getValues().raw(Units::TemperatureUnit::Celcius).back()
                                    : 0;
        float tankPressure_psi = GSDataCenter::PressureSensor_Motor_PlotData.tank().getSize()
                                   ? GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData().getValues().raw(Units::PressureUnit::Psi).back()
                                   : 0;
        float tankMass_lb = GSDataCenter::NOSTankMass_PlotData.getSize() ? GSDataCenter::NOSTankMass_PlotData.getValues().raw(Units::WeightUnit::Pounds).back() : 0;
        const double vaporPressure_psi = VaporPressure::vaporPressureNOS_psi(tankTemperature_C);
        const std::string phaseStr = tankPressure_psi > vaporPressure_psi ? "Liquid" : "Gas";

        float tankTemperature = Units::convert(tankTemperature_C, Units::TemperatureUnit::Celcius, Units::DEFAULT_TEMPERATURE_UNIT);
        float tankPressure = Units::convert(tankPressure_psi, Units::PressureUnit::Psi, Units::DEFAULT_PRESSURE_UNIT);
        float tankMass = Units::convert(tankMass_lb, Units::WeightUnit::Pounds, Units::DEFAULT_WEIGHT_UNIT);

        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Temp Tank (%s): %f", Units::as_symbol(Units::DEFAULT_TEMPERATURE_UNIT), tankTemperature);
        ImGui::Text("PT Tank (%s): %f", Units::as_symbol(Units::DEFAULT_FORCE_UNIT), tankPressure);
        ImGui::Text("LC Tank (%s): %f", Units::as_symbol(Units::DEFAULT_WEIGHT_UNIT), tankMass);
        ImGui::Text("Phase: %s", phaseStr.c_str());

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginTable("Plots", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankPressurePlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes(Units::as_label(TIME_UNIT), Units::as_label(Units::DEFAULT_PRESSURE_UNIT));
                recentMotorPressureSensor1.plot(TIME_UNIT, Units::DEFAULT_PRESSURE_UNIT, false);
                recentMotorPressureSensor2.plot(TIME_UNIT, Units::DEFAULT_PRESSURE_UNIT, false);
                recentFillPressureSensor1.plot(TIME_UNIT, Units::DEFAULT_PRESSURE_UNIT, false);
                recentFillPressureSensor2.plot(TIME_UNIT, Units::DEFAULT_PRESSURE_UNIT, false);
                ImPlot::EndPlot();
            }
            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankTemperaturePlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes(Units::as_label(TIME_UNIT), Units::as_label(Units::DEFAULT_TEMPERATURE_UNIT));
                recentTankTemperature.plot(TIME_UNIT, Units::DEFAULT_TEMPERATURE_UNIT, false);
                ImPlot::EndPlot();
            }

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(engineThrustPlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes(Units::as_label(TIME_UNIT), Units::as_label(Units::DEFAULT_FORCE_UNIT));
                recentEngineThrust.plot(TIME_UNIT, Units::DEFAULT_FORCE_UNIT);
                ImPlot::EndPlot();
            }

            ImGui::TableSetColumnIndex(1);
            ImPlot::SetNextAxesToFit();
            if (ImPlot::BeginPlot(tankMassPlotTitle.c_str(), ImVec2(-1, plotRowHeight), ImPlotFlags_NoInputs)) {
                ImPlot::SetupAxes(Units::as_label(TIME_UNIT), Units::as_label(Units::DEFAULT_WEIGHT_UNIT));
                recentTankMass.plot(TIME_UNIT, Units::DEFAULT_WEIGHT_UNIT);
                // recentTankLoadCell.plot(); TODO : Temporarily commented out for tests
                ImPlot::EndPlot();
            }

            ImGui::EndTable();
        }

        ImGui::EndTable();
    }
}

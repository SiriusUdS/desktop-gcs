#include "MonitoringWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "ResultsWindow.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "imgui.h"

#include <implot.h>

const char* const MonitoringWindow::name = "Monitoring";

MonitoringWindow::MonitoringWindow()
    : tankGasLeftPlotLine{GSDataCenter::TankGasLeft_perc_PlotData, PlotStyle("Tank Gas Left", ThemedColors::PlotLine::blue)},
      thrustLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                             PlotStyle("Thrust Load Cell", ThemedColors::PlotLine::red)},
      tankPressurePlotLine{GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData(),
                           PlotStyle("Tank Pressure", ThemedColors::PlotLine::blue)},
      chamberPressurePlotLine{GSDataCenter::PressureSensor_Motor_PlotData.p2().getValuePlotData(),
                              PlotStyle("Chamber Pressure", ThemedColors::PlotLine::red)},
      motorThermistorPlotLines{
        {{GSDataCenter::Thermistor_Motor_PlotData.t1().getValuePlotData(), PlotStyle("Motor Thermistor 1", ThemedColors::PlotLine::blue)},
         {GSDataCenter::Thermistor_Motor_PlotData.t2().getValuePlotData(), PlotStyle("Motor Thermistor 2", ThemedColors::PlotLine::red)},
         {GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData(), PlotStyle("Motor Thermistor 3", ThemedColors::PlotLine::green)},
         {GSDataCenter::Thermistor_Motor_PlotData.t4().getValuePlotData(), PlotStyle("Motor Thermistor 4", ThemedColors::PlotLine::yellow)},
         {GSDataCenter::Thermistor_Motor_PlotData.t5().getValuePlotData(), PlotStyle("Motor Thermistor 5", ThemedColors::PlotLine::orange)},
         {GSDataCenter::Thermistor_Motor_PlotData.t6().getValuePlotData(), PlotStyle("Motor Thermistor 6", ThemedColors::PlotLine::cyan)},
         {GSDataCenter::Thermistor_Motor_PlotData.t7().getValuePlotData(), PlotStyle("Motor Thermistor 7", ThemedColors::PlotLine::purple)},
         {GSDataCenter::Thermistor_Motor_PlotData.t8().getValuePlotData(), PlotStyle("Motor Thermistor 8", ThemedColors::PlotLine::gray)}}} {
}

const char* MonitoringWindow::getName() const {
    return name;
}

const char* MonitoringWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void MonitoringWindow::renderImpl() {
    ImGui::SeparatorText("Monitoring");
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Gas Left, Load Cells and Chamber Pressure", {-1.0f, 600.0f})) {
        constexpr ImAxis gasLeftAxis = ImAxis_Y1;
        constexpr ImAxis weightAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, "Timestamp (ms)");
        ImPlot::SetupAxis(gasLeftAxis, "Gas Left (%)");
        ImPlot::SetupAxis(weightAxis, "Weight (lb)");

        ImPlot::SetAxis(gasLeftAxis);
        tankGasLeftPlotLine.plot();
        ImPlot::SetAxis(weightAxis);
        thrustLoadCellPlotLine.plot();

        ImPlot::EndPlot();
    }

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Temperature and Pressure", {-1.0f, 600.0f})) {
        constexpr ImAxis pressureAxis = ImAxis_Y1;
        constexpr ImAxis temperatureAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, "Timestamp (ms)");
        ImPlot::SetupAxis(pressureAxis, "Pressure (psi)");
        ImPlot::SetupAxis(temperatureAxis, "Temperature (C)");

        ImPlot::SetAxis(pressureAxis);
        tankPressurePlotLine.plot();
        chamberPressurePlotLine.plot();

        ImPlot::SetAxis(temperatureAxis);
        for (const PlotLine& plotLine : motorThermistorPlotLines) {
            plotLine.plot();
        }
        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Switch to \"Results\"");
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(ResultsWindow::name);
    }
}

#include "MonitoringWindow.h"

#include "GSDataCenter.h"
#include "IniParams.h"
#include "ResultsWindow.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "units.h"
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

void MonitoringWindow::renderImpl() {
    constexpr Units::TimeUnit TIME_UNIT = Units::TimeUnit::Seconds;
    constexpr Units::WeightUnit WEIGHT_UNIT = Units::DEFAULT_WEIGHT_UNIT;
    constexpr Units::PressureUnit PRESSURE_UNIT = Units::DEFAULT_PRESSURE_UNIT;
    constexpr Units::TemperatureUnit TEMPERATURE_UNIT = Units::DEFAULT_TEMPERATURE_UNIT;

    ImGui::SeparatorText("Monitoring");
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Gas Left, Load Cells and Chamber Pressure", {-1.0f, 600.0f})) {
        constexpr ImAxis gasLeftAxis = ImAxis_Y1;
        constexpr ImAxis weightAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, Units::as_label(TIME_UNIT));
        ImPlot::SetupAxis(gasLeftAxis, Units::as_label(Units::QuantityUnit::Percentage));
        ImPlot::SetupAxis(weightAxis, Units::as_label(WEIGHT_UNIT));

        ImPlot::SetAxis(gasLeftAxis);
        tankGasLeftPlotLine.plot(TIME_UNIT, Units::QuantityUnit::Percentage, IniParams::compressPlots.currentValue);
        ImPlot::SetAxis(weightAxis);
        thrustLoadCellPlotLine.plot(TIME_UNIT, WEIGHT_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::EndPlot();
    }

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Temperature and Pressure", {-1.0f, 600.0f})) {
        constexpr ImAxis pressureAxis = ImAxis_Y1;
        constexpr ImAxis temperatureAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, Units::as_label(TIME_UNIT));
        ImPlot::SetupAxis(pressureAxis, Units::as_label(PRESSURE_UNIT));
        ImPlot::SetupAxis(temperatureAxis, Units::as_label(TEMPERATURE_UNIT));

        ImPlot::SetAxis(pressureAxis);
        tankPressurePlotLine.plot(TIME_UNIT, PRESSURE_UNIT, IniParams::compressPlots.currentValue);
        chamberPressurePlotLine.plot(TIME_UNIT, PRESSURE_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::SetAxis(temperatureAxis);
        for (const PlotLine& plotLine : motorThermistorPlotLines) {
            plotLine.plot(TIME_UNIT, TEMPERATURE_UNIT, IniParams::compressPlots.currentValue);
        }
        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Switch to \"Results\"");
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(ResultsWindow::name);
    }
}

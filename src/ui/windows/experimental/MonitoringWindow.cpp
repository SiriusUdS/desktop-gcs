#include "MonitoringWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"

#include <implot.h>

MonitoringWindow::MonitoringWindow()
    : thrustLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                             PlotStyle("Thrust Load Cell", ThemedColors::PlotLine::blue)},
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

const char* MonitoringWindow::name() const {
    return "Monitoring";
}

const char* MonitoringWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void MonitoringWindow::renderImpl() {
    if (ImPlot::BeginPlot("Load Cells", {-1.0f, 500.0f})) {
        thrustLoadCellPlotLine.plot();
        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Temperature and Pressure", {-1.0f, 500.0f})) {
        for (const PlotLine& plotLine : motorThermistorPlotLines) {
            plotLine.plot();
        }
        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Switch to \"Results\"");
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus("Results"); // TODO: Access window name from results window class
    }
}

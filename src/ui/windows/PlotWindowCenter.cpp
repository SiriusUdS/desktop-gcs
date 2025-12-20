#include "PlotWindowCenter.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "ThemedColors.h"

namespace PlotWindowCenter {
constexpr const char* TIMESTAMP_AXIS_NAME = "Timestamp (ms)";

PlotWindow thermistorMotorPlot("Motor Thermistors",
                               TIMESTAMP_AXIS_NAME,
                               "Temperature (C)",
                               {{GSDataCenter::Thermistor_Motor_PlotData.t1(), PlotStyle("Thermistor 1", ThemedColors::PlotLine::blue)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t2(), PlotStyle("Thermistor 2", ThemedColors::PlotLine::red)},
                                {GSDataCenter::Thermistor_Motor_PlotData.tank(), PlotStyle("Tank Thermistor", ThemedColors::PlotLine::green)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t4(), PlotStyle("Thermistor 4", ThemedColors::PlotLine::yellow)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t5(), PlotStyle("Thermistor 5", ThemedColors::PlotLine::orange)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t6(), PlotStyle("Thermistor 6", ThemedColors::PlotLine::cyan)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t7(), PlotStyle("Thermistor 7", ThemedColors::PlotLine::purple)},
                                {GSDataCenter::Thermistor_Motor_PlotData.t8(), PlotStyle("Thermistor 8", ThemedColors::PlotLine::gray)}});

PlotWindow
  pressureSensorMotorPlot("Motor Pressure Sensors",
                          TIMESTAMP_AXIS_NAME,
                          "Pressure (psi)",
                          {{GSDataCenter::PressureSensor_Motor_PlotData.tank(), PlotStyle("Pressure Sensor 1 (Motor)", ThemedColors::PlotLine::blue)},
                           {GSDataCenter::PressureSensor_Motor_PlotData.p2(), PlotStyle("Pressure Sensor 2 (Motor)", ThemedColors::PlotLine::red)}});

PlotWindow
  thermistorFillingStationPlot("Filling Station Thermistors",
                               TIMESTAMP_AXIS_NAME,
                               "Temperature (C)",
                               {{GSDataCenter::Thermistor_FillingStation_PlotData.t1(), PlotStyle("Thermistor 1", ThemedColors::PlotLine::blue)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t2(), PlotStyle("Thermistor 2", ThemedColors::PlotLine::red)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t3(), PlotStyle("Thermistor 3", ThemedColors::PlotLine::green)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t4(), PlotStyle("Thermistor 4", ThemedColors::PlotLine::yellow)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t5(), PlotStyle("Thermistor 5", ThemedColors::PlotLine::orange)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t6(), PlotStyle("Thermistor 6", ThemedColors::PlotLine::cyan)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t7(), PlotStyle("Thermistor 7", ThemedColors::PlotLine::purple)},
                                {GSDataCenter::Thermistor_FillingStation_PlotData.t8(), PlotStyle("Thermistor 8", ThemedColors::PlotLine::gray)}});

PlotWindow pressureSensorFillingStationPlot(
  "Filling Station Pressure Sensors",
  TIMESTAMP_AXIS_NAME,
  "Pressure (psi)",
  {{GSDataCenter::PressureSensor_FillingStation_PlotData.p1(), PlotStyle("Pressure Sensor 1 (Fill)", ThemedColors::PlotLine::blue)},
   {GSDataCenter::PressureSensor_FillingStation_PlotData.p2(), PlotStyle("Pressure Sensor 2 (Fill)", ThemedColors::PlotLine::red)}});

PlotWindow
  loadCellFillingStationPlot("Filling Station Load Cells",
                             TIMESTAMP_AXIS_NAME,
                             "Weight (lb)",
                             {{GSDataCenter::LoadCell_FillingStation_PlotData.motor(), PlotStyle("Motor Load Cell", ThemedColors::PlotLine::blue)},
                              {GSDataCenter::LoadCell_FillingStation_PlotData.tank(), PlotStyle("Tank Load Cell", ThemedColors::PlotLine::red)}});
} // namespace PlotWindowCenter

std::vector<HelloImGui::DockableWindow> PlotWindowCenter::createDockableWindows() {
    HelloImGui::DockableWindow thermistorMotorPlotDockWin(thermistorMotorPlot.getWindowId(), ImGuiConfig::Dockspace::PLOT, [] {
        PlotWindowCenter::thermistorMotorPlot.render();
    });
    HelloImGui::DockableWindow pressureSensorMotorPlotDockWin(pressureSensorMotorPlot.getWindowId(), ImGuiConfig::Dockspace::PLOT, [] {
        PlotWindowCenter::pressureSensorMotorPlot.render();
    });
    HelloImGui::DockableWindow thermistorFillingStationPlotDockWin(thermistorFillingStationPlot.getWindowId(), ImGuiConfig::Dockspace::PLOT, [] {
        PlotWindowCenter::thermistorFillingStationPlot.render();
    });
    HelloImGui::DockableWindow pressureSensorFillingStationPlotDockWin(pressureSensorFillingStationPlot.getWindowId(),
                                                                       ImGuiConfig::Dockspace::PLOT,
                                                                       [] { PlotWindowCenter::pressureSensorFillingStationPlot.render(); });
    HelloImGui::DockableWindow loadCellFillingStationPlotDockWin(loadCellFillingStationPlot.getWindowId(), ImGuiConfig::Dockspace::PLOT, [] {
        PlotWindowCenter::loadCellFillingStationPlot.render();
    });

    return {thermistorMotorPlotDockWin,
            pressureSensorMotorPlotDockWin,
            thermistorFillingStationPlotDockWin,
            pressureSensorFillingStationPlotDockWin,
            loadCellFillingStationPlotDockWin};
}

void PlotWindowCenter::loadState(const mINI::INIStructure& iniStructure) {
    thermistorMotorPlot.loadState(iniStructure);
    pressureSensorMotorPlot.loadState(iniStructure);
    thermistorFillingStationPlot.loadState(iniStructure);
    pressureSensorFillingStationPlot.loadState(iniStructure);
    loadCellFillingStationPlot.loadState(iniStructure);
}

void PlotWindowCenter::saveState(mINI::INIStructure& iniStructure) {
    thermistorMotorPlot.saveState(iniStructure);
    pressureSensorMotorPlot.saveState(iniStructure);
    thermistorFillingStationPlot.saveState(iniStructure);
    pressureSensorFillingStationPlot.saveState(iniStructure);
    loadCellFillingStationPlot.saveState(iniStructure);
}

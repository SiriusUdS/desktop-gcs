#include "PlotWindowCenter.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"

namespace PlotWindowCenter {
constexpr const char* TIMESTAMP_AXIS_NAME = "Timestamp (ms)";

PlotWindow thermistorMotorPlot("Motor Thermistors", TIMESTAMP_AXIS_NAME, "Temperature (C)",
                               {&GSDataCenter::Thermistor_Motor_PlotData[0], &GSDataCenter::Thermistor_Motor_PlotData[1],
                                &GSDataCenter::Thermistor_Motor_PlotData[2], &GSDataCenter::Thermistor_Motor_PlotData[3],
                                &GSDataCenter::Thermistor_Motor_PlotData[4], &GSDataCenter::Thermistor_Motor_PlotData[5],
                                &GSDataCenter::Thermistor_Motor_PlotData[6], &GSDataCenter::Thermistor_Motor_PlotData[7]});

PlotWindow pressureSensorMotorPlot("Motor Pressure Sensors", TIMESTAMP_AXIS_NAME, "Pressure (psi)",
                                   {&GSDataCenter::PressureSensor_Motor_PlotData[0], &GSDataCenter::PressureSensor_Motor_PlotData[1]});

PlotWindow thermistorFillingStationPlot("Filling Station Thermistors", TIMESTAMP_AXIS_NAME, "Temperature (C)",
                                        {&GSDataCenter::Thermistor_FillingStation_PlotData[0], &GSDataCenter::Thermistor_FillingStation_PlotData[1],
                                         &GSDataCenter::Thermistor_FillingStation_PlotData[2], &GSDataCenter::Thermistor_FillingStation_PlotData[3],
                                         &GSDataCenter::Thermistor_FillingStation_PlotData[4], &GSDataCenter::Thermistor_FillingStation_PlotData[5],
                                         &GSDataCenter::Thermistor_FillingStation_PlotData[6], &GSDataCenter::Thermistor_FillingStation_PlotData[7]});

PlotWindow pressureSensorFillingStationPlot("Filling Station Pressure Sensors", TIMESTAMP_AXIS_NAME, "Pressure (psi)",
                                            {&GSDataCenter::PressureSensor_FillingStation_PlotData[0],
                                             &GSDataCenter::PressureSensor_FillingStation_PlotData[1]});

PlotWindow loadCellFillingStationPlot("Filling Station Load Cells", TIMESTAMP_AXIS_NAME, "Weight (lb)",
                                      {&GSDataCenter::LoadCell_FillingStation_PlotData[0], &GSDataCenter::LoadCell_FillingStation_PlotData[1]});
} // namespace PlotWindowCenter

std::vector<HelloImGui::DockableWindow> PlotWindowCenter::createDockableWindows() {
    // clang-format off
    HelloImGui::DockableWindow thermistorMotorPlotDockWin               (thermistorMotorPlot.getWindowId(),              ImGuiConfig::Dockspace::PLOT, []() { PlotWindowCenter::thermistorMotorPlot.render(); });
    HelloImGui::DockableWindow pressureSensorMotorPlotDockWin           (pressureSensorMotorPlot.getWindowId(),          ImGuiConfig::Dockspace::PLOT, []() { PlotWindowCenter::pressureSensorMotorPlot.render(); });
    HelloImGui::DockableWindow thermistorFillingStationPlotDockWin      (thermistorFillingStationPlot.getWindowId(),     ImGuiConfig::Dockspace::PLOT, []() { PlotWindowCenter::thermistorFillingStationPlot.render(); });
    HelloImGui::DockableWindow pressureSensorFillingStationPlotDockWin  (pressureSensorFillingStationPlot.getWindowId(), ImGuiConfig::Dockspace::PLOT, []() { PlotWindowCenter::pressureSensorFillingStationPlot.render(); });
    HelloImGui::DockableWindow loadCellFillingStationPlotDockWin        (loadCellFillingStationPlot.getWindowId(),       ImGuiConfig::Dockspace::PLOT, []() { PlotWindowCenter::loadCellFillingStationPlot.render(); });
    // clang-format on

    return {thermistorMotorPlotDockWin, pressureSensorMotorPlotDockWin, thermistorFillingStationPlotDockWin, pressureSensorFillingStationPlotDockWin,
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

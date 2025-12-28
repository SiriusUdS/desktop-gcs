#include "Application.h"

// clang-format off
#include <WinSock2.h>
// clang-format on

#include "BoardsWindow.h"
#include "ConfigParamsIO.h"
#include "ControlsWindow.h"
#include "FillWindow.h"
#include "FontConfig.h"
#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "LaunchWindow.h"
#include "Logging.h"
#include "LoggingWindow.h"
#include "MapWindow.h"
#include "MonitoringWindow.h"
#include "NOSPhaseDiagramWindow.h"
#include "PlotWindowCenter.h"
#include "PrefillWindow.h"
#include "PrelaunchWindow.h"
#include "ResultsWindow.h"
#include "RocketParametersWindow.h"
#include "SensorPlotData.h"
#include "SerialComWindow.h"
#include "SerialTask.h"
#include "SwitchesWindow.h"
#include "TankMassCalculatorWindow.h"
#include "TankMassPlotDataUpdater.h"
#include "TankMassWindow.h"
#include "UITheme.h"
#include "ValvesWindow.h"

#include <imgui.h>
#include <implot.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

namespace Application {
mINI::INIFile iniFile("sirius_gcs.ini");
mINI::INIStructure iniStructure;
TankMassPlotDataUpdater tankMassPlotDataUpdater{
  {&GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData(), &GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData()}};
std::vector<std::shared_ptr<UIWindow>> windows;
std::shared_ptr<LoggingWindow> loggingWindow;
} // namespace Application

void Application::loadFonts() {
    const std::string mainFontPath = "fonts/Nunito-Regular.ttf";
    const std::string boldMainFontPath = "fonts/Nunito-Bold.ttf";
    const std::string monospaceFontPath = "fonts/Consolas-Regular.ttf";

    FontConfig::mainFont = HelloImGui::LoadFont(mainFontPath, 24.f);
    HelloImGui::MergeFontAwesomeToLastFont(20.f);
    FontConfig::boldMainFont = HelloImGui::LoadFont(boldMainFontPath, 24.f);
    FontConfig::monospaceFont = HelloImGui::LoadFont(monospaceFontPath, 20.f);
}

void Application::init() {
    Logging::init();

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        GCS_APP_LOG_ERROR("WSAStartup failed.");
        return;
    }

    ImPlot::CreateContext();

    iniFile.read(iniStructure);

    PlotWindowCenter::loadState(iniStructure);
    ConfigParamsIO::loadParams(iniStructure);

    loggingWindow = std::make_shared<LoggingWindow>();

    // windows.emplace_back(std::make_shared<BoardsWindow>());
    // windows.emplace_back(std::make_shared<ControlsWindow>());
    windows.emplace_back(loggingWindow);
    windows.emplace_back(std::make_shared<MapWindow>());
    // windows.emplace_back(std::make_shared<NOSPhaseDiagramWindow>());
    windows.emplace_back(std::make_shared<RocketParametersWindow>());
    windows.emplace_back(std::make_shared<SerialComWindow>());
    // windows.emplace_back(std::make_shared<SwitchesWindow>());
    windows.emplace_back(std::make_shared<TankMassCalculatorWindow>());
    windows.emplace_back(std::make_shared<TankMassWindow>());
    // windows.emplace_back(std::make_shared<ValvesWindow>());

    windows.emplace_back(std::make_shared<PrefillWindow>());
    windows.emplace_back(std::make_shared<FillWindow>());
    windows.emplace_back(std::make_shared<PrelaunchWindow>());
    windows.emplace_back(std::make_shared<LaunchWindow>());
    windows.emplace_back(std::make_shared<MonitoringWindow>());
    windows.emplace_back(std::make_shared<ResultsWindow>());

    for (const auto& window : windows) {
        window->init();
        window->loadState(iniStructure);
    }

    tankMassPlotDataUpdater.subscribe();

    SerialTask::start();
}

void Application::preNewFrame() {
    UITheme::update();
}

void Application::shutdown() {
    SerialTask::stop();
    ConfigParamsIO::saveParams(iniStructure);
    PlotWindowCenter::saveState(iniStructure);

    for (const auto& window : windows) {
        window->saveState(iniStructure);
    }

    iniFile.write(iniStructure);

    ImPlot::DestroyContext();

    WSACleanup();
}

std::vector<HelloImGui::DockingSplit> Application::createBaseDockingSplits() {
    HelloImGui::DockingSplit splitPlotLogs;
    splitPlotLogs.initialDock = ImGuiConfig::Dockspace::PLOT;
    splitPlotLogs.newDock = ImGuiConfig::Dockspace::LOGGING;
    splitPlotLogs.direction = ImGuiDir_Down;
    splitPlotLogs.ratio = 0.33f;

    HelloImGui::DockingSplit splitPlotMap;
    splitPlotMap.initialDock = ImGuiConfig::Dockspace::PLOT;
    splitPlotMap.newDock = ImGuiConfig::Dockspace::MAP;
    splitPlotMap.direction = ImGuiDir_Left;
    splitPlotMap.ratio = 0.5f;

    std::vector<HelloImGui::DockingSplit> splits = {splitPlotLogs, splitPlotMap};
    return splits;
}

std::vector<HelloImGui::DockableWindow> Application::createDockableWindows() {
    std::vector<HelloImGui::DockableWindow> dockableWindows = PlotWindowCenter::createDockableWindows();

    for (const auto& window : windows) {
        dockableWindows.emplace_back(window->getName(), window->getDockspace(), [window]() { window->render(); });
    }

    return dockableWindows;
}

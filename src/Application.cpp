#include "Application.h"

// clang-format off
#include <WinSock2.h>
// clang-format on

#include "FontConfig.h"
#include "GSDataCenter.h"
#include "IniParams.h"
#include "IniParamsIO.h"
#include "Logging.h"
#include "PlotWindowCenter.h"
#include "SerialTask.h"
#include "TankGasLeftPlotDataProcessor.h"
#include "TankMassPlotDataProcessor.h"
#include "UITheme.h"
#include "UIWindow.h"
#include "UIWindows.h"

#include <imgui.h>
#include <implot.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

namespace Application {
mINI::INIFile iniFile("sirius_gcs.ini");
mINI::INIStructure iniStructure;
TankGasLeftPlotDataProcessor tankGasLeftPlotDataProcessor;
TankMassPlotDataProcessor tankMassPlotDataProcessor;
std::vector<std::shared_ptr<UIWindow>> windows;
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
    IniParamsIO::loadParams(iniStructure);

    UIWindows::init();
    UIWindows::loadState(iniStructure);

    tankGasLeftPlotDataProcessor.subscribe();
    tankMassPlotDataProcessor.subscribe();

    SerialTask::start();
}

void Application::preNewFrame() {
    UITheme::update();
}

void Application::showMenus() {
    if (ImGui::BeginMenu("Plot")) {
        if (ImGui::MenuItem("Show compressed data", NULL, IniParams::compressPlots.currentValue)) {
            IniParams::compressPlots.currentValue = !IniParams::compressPlots.currentValue.load();
        }

        if (ImGui::MenuItem("Clear all")) {
            for (SensorPlotData& sensorPlotData : GSDataCenter::Thermistor_Motor_PlotData.data) {
                sensorPlotData.clear();
            }
            for (SensorPlotData& sensorPlotData : GSDataCenter::PressureSensor_Motor_PlotData.data) {
                sensorPlotData.clear();
            }
            for (SensorPlotData& sensorPlotData : GSDataCenter::Thermistor_FillingStation_PlotData.data) {
                sensorPlotData.clear();
            }
            for (SensorPlotData& sensorPlotData : GSDataCenter::PressureSensor_FillingStation_PlotData.data) {
                sensorPlotData.clear();
            }
            for (SensorPlotData& sensorPlotData : GSDataCenter::LoadCell_FillingStation_PlotData.data) {
                sensorPlotData.clear();
            }
            GSDataCenter::NOSTankMass_PlotData.clear();
            GSDataCenter::TankGasLeft_perc_PlotData.clear();
        }

        ImGui::EndMenu();
    }
}

void Application::shutdown() {
    SerialTask::stop();
    IniParamsIO::saveParams(iniStructure);
    PlotWindowCenter::saveState(iniStructure);

    UIWindows::saveState(iniStructure);

    iniFile.write(iniStructure);

    ImPlot::DestroyContext();

    WSACleanup();
}

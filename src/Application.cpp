#include "Application.h"

// clang-format off
#include <WinSock2.h>
// clang-format on

#include "ComTask.h"
#include "FontConfig.h"
#include "GSDataCenter.h"
#include "IniParams.h"
#include "IniParamsIO.h"
#include "Logging.h"
// #include "PlotWindowCenter.h" // plot windows deactivated (not compiled)
// #include "SerialCom.h" // SERIAL PATH RETIRED (UDP-only; UdpCom is hardcoded below)
#include "TankGasLeftPlotDataProcessor.h"
#include "TankMassPlotDataProcessor.h"
#include "data/data_conversion/equationHandler.h"
#include "UdpCom.h"
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

    // PlotWindowCenter::loadState(iniStructure); // plot windows deactivated (not compiled)
    IniParamsIO::loadParams(iniStructure);
    // CameraManager::get().init(); // camera deactivated (not compiled)
    UIWindows::init();
    UIWindows::loadState(iniStructure);

    tankGasLeftPlotDataProcessor.subscribe();
    tankMassPlotDataProcessor.subscribe();
       
    //Switch com type here
    std::unique_ptr<UdpCom> comProtocol = std::make_unique<UdpCom>();
    ComTask::start(std::move(comProtocol));
}

void Application::preNewFrame() {
    UITheme::update();
    // CameraManager::get().processFrameAndUploadToGPU(); // camera deactivated (not compiled)
}

struct EquationUIRow {
    char key[64] = {};
    char equation[256] = {};
};

void Application::showMenus() {
    bool openOffsetPopup = false;
    bool openEquationPopup = false;
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

    if (ImGui::BeginMenu("Calibration")) {
        if (ImGui::MenuItem("Offset values")) {
            openOffsetPopup = true; // Trigger the flag instead of opening directly
        }

        if (ImGui::MenuItem("Equations")) {
            openEquationPopup = true; // Trigger the flag
        }
        ImGui::EndMenu();
    }

    // 2. Safely call OpenPopup outside the menu scope
    if (openOffsetPopup) {
        ImGui::OpenPopup("OffsetPopup");
    }
    if (openEquationPopup) {
        ImGui::OpenPopup("EquationPopup");
    }

    // 3. Render the popups
    if (ImGui::BeginPopup("OffsetPopup")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        static char tank[256] = {};
        static char chamber[256] = {};
        static char pressure[256] = {};

        // Trigger this only on the exact frame the popup appears
        if (ImGui::IsWindowAppearing()) {
            equations::EquationHandler& eHandler = equations::EquationHandler::getInstance();

            // Assuming you have a getEquation method.
            // strncpy ensures we don't overflow the 256-byte buffers.
            strncpy(tank, eHandler.getEquation("tankOffset").c_str(), sizeof(tank) - 1);
            strncpy(chamber, eHandler.getEquation("chamberOffset").c_str(), sizeof(chamber) - 1);
            strncpy(pressure, eHandler.getEquation("pressureOffset").c_str(), sizeof(pressure) - 1);
        }

        ImGui::Text("Tank Mass Offset:");
        ImGui::InputText("##Tank offset", tank, sizeof(tank));

        ImGui::Text("Trust Offset:");
        ImGui::InputText("##Chamber offset", chamber, sizeof(chamber));

        ImGui::Text("Tank Pressure Offset:");
        ImGui::InputText("##pressure offset", pressure, sizeof(pressure));

        if (ImGui::Button("Add")) {
            equations::EquationHandler& eHandler = equations::EquationHandler::getInstance();
            eHandler.setEquation("tankOffset", std::string(tank));
            eHandler.setEquation("chamberOffset", std::string(chamber));
            eHandler.setEquation("pressureOffset", std::string(pressure));
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar();

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("EquationPopup")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::Text("Equation List (variable is x)");
        ImGui::Separator();

        // 1. Maintain a static vector to act as our temporary UI buffer
        static std::vector<EquationUIRow> uiRows;

        // 2. Populate the UI buffer ONLY when the window first opens
        if (ImGui::IsWindowAppearing()) {
            uiRows.clear();
            equations::EquationHandler& eHandler = equations::EquationHandler::getInstance();
            auto keys = eHandler.getKeys();

            for (const auto& key : keys) {
                if (key.find("Offset") != std::string::npos) {
                    continue;
                }
                EquationUIRow row;
                std::string eq = eHandler.getEquation(key);

                // Safely copy strings into our char buffers
                strncpy(row.key, key.c_str(), sizeof(row.key) - 1);
                strncpy(row.equation, eq.c_str(), sizeof(row.equation) - 1);

                uiRows.push_back(row);
            }
        }

        // Variable to track if a row needs to be deleted this frame
        int rowToDelete = -1;

        // 3. Render the rows
        for (int i = 0; i < uiRows.size(); ++i) {
            // CRITICAL: Push ID so ImGui can differentiate these textboxes from the others
            ImGui::PushID(i);

            // Key Input
            ImGui::SetNextItemWidth(120.0f); // Restrict width so they fit on one line
            ImGui::InputText("##key", uiRows[i].key, sizeof(uiRows[i].key));

            ImGui::SameLine();
            ImGui::Text("="); // Visual separator
            ImGui::SameLine();

            // Equation Input
            ImGui::SetNextItemWidth(250.0f);
            ImGui::InputText("##equation", uiRows[i].equation, sizeof(uiRows[i].equation));

            // Optional but highly recommended: A way to remove an equation
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                rowToDelete = i;
            }

            ImGui::PopID();
        }

        // Handle deletion outside the loop so we don't break the iterator
        if (rowToDelete != -1) {
            equations::EquationHandler::getInstance().removeEquation(uiRows[rowToDelete].key);
            uiRows.erase(uiRows.begin() + rowToDelete);
            
        }

        ImGui::Separator();

        // 4. Action Buttons
        if (ImGui::Button("Add Equation")) {
            uiRows.push_back(EquationUIRow()); // Adds a new blank row
        }

        ImGui::SameLine();

        if (ImGui::Button("Save")) {
            equations::EquationHandler& eHandler = equations::EquationHandler::getInstance();

            // NOTE: If your handler has a clear() method, you should call it here
            // to prevent orphaned equations if the user renamed a key.
            // eHandler.clear();

            for (const auto& row : uiRows) {
                std::string k(row.key);
                std::string eq(row.equation);

                // Only save if the key isn't empty
                if (!k.empty()) {
                    eHandler.setEquation(k, eq);
                }
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup(); // Discards changes by just closing
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }
}

void Application::shutdown() {
    ComTask::stop();
    IniParamsIO::saveParams(iniStructure);
    // PlotWindowCenter::saveState(iniStructure); // plot windows deactivated (not compiled)

    UIWindows::saveState(iniStructure);

    iniFile.write(iniStructure);

    ImPlot::DestroyContext();

    WSACleanup();
}

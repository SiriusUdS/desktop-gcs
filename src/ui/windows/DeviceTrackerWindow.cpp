#include "DeviceTrackerWindow.h"

#include "ComTask.h"

#include <bit>
#include <imgui.h>
const char* const DeviceTrackerWindow::name = "Devices";


const char* DeviceTrackerWindow::getName() const {
    return "Devices";
}

void DeviceTrackerWindow::renderImpl() {
    if (!ComTask::deviceTracker) {
        ImGui::Text("UDP Connection not established. Waiting for Device Tracker...");
        return;
    }

    static std::unordered_map<uint8_t, bool> logWindowStates;
    
    std::vector<DeviceInformation> activeDevices = ComTask::getAllDeviceInformation();

    if (activeDevices.empty()) {
        ImGui::Text("No active devices detected...");
        return;
    }
    
    std::sort(activeDevices.begin(), activeDevices.end(), [](const DeviceInformation& a, const DeviceInformation& b) {
        return a.deviceTsMs > b.deviceTsMs; 
    });
    
    for (const auto& state : activeDevices) {
        ImGui::PushID(state.deviceID); 
        
        char headerName[64];
        sprintf_s(headerName, sizeof(headerName), "Device %d", state.deviceID);

        if (ImGui::CollapsingHeader(headerName, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Device ID: %d", state.deviceID);
            ImGui::Text("Last TS: %u ms", state.deviceTsMs);
            ImGui::Text("State: %d", state.deviceStatus);
            uint8_t rawFlags = std::bit_cast<uint8_t>(state.deviceCtrlFlags);
            ImGui::Text("Error flag: %u", rawFlags);

            ImGui::Spacing();
            ImGui::Text("Logs:");

            if (ImGui::BeginChild("LogRegion", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar)) {
                std::vector<std::string> logs = ComTask::deviceTracker->getDeviceLogs(state.deviceID);
                
                for (const auto& log : logs) {
                    ImGui::TextUnformatted(log.c_str());
                }

                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
            }
            ImGui::EndChild();
        }
        
        ImGui::PopID();
    }
}
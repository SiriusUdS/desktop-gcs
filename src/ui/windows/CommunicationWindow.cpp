#include "CommunicationWindow.h"
#include "ComTask.h"
#include "GSDataCenter.h"
#include "ICom.h"
#include "ITileLoader.h"
#include "Logging.h"
#include "UdpConfig.h"

#include <cstring>

const char* const CommunicationWindow::name = "Communication";


const char* CommunicationWindow::getName() const {
    return "Communication";
}

void CommunicationWindow::renderImpl() {
    static bool loggingEnabled = false;
    static uint64_t lostPacketCount = 0;
    
    if (ImGui::CollapsingHeader("UDP Configuration", ImGuiTreeNodeFlags_DefaultOpen)) {
        static char ipBuf[64];
        static int listenerPort = UdpConfig::defaultReceivePort;
        static int senderPort = UdpConfig::defaultDestPort;
        static bool ipBufInitialized = false;
        
        if (!ipBufInitialized) {
            strcpy_s(ipBuf, UdpConfig::defaultDestIp);
            ipBufInitialized = true;
        }

        ImGui::InputText("Remote IP", ipBuf, IM_ARRAYSIZE(ipBuf));
        ImGui::InputInt("Listener Port", &listenerPort);
        ImGui::InputInt("Sender Port", &senderPort);

        if (ImGui::Button("Update Connection", ImVec2(-FLT_MIN, 0))) {
            if (ComTask::updateConnection(ipBuf, static_cast<uint16_t>(senderPort), static_cast<uint16_t>(listenerPort))) {
                //GCS_APP_LOG_INFO("Updated connection successfully to: {}:{}", ipBuf, senderPort);
            } else {
                GCS_APP_LOG_ERROR("Failed to update connection");
            }
        }
        
        if (ComTask::com->getComType() == ComType::UDP) {
            uint64_t totalReceived = ComTask::getTotalReceivedPackets();
            uint64_t lostPackets = ComTask::getLostPacketCount();
            
            ImGui::Text("Amount of lost packets: %llu", lostPackets);
            ImGui::Text("Total received packets: %llu", totalReceived);
            
            float percent = 100.0f;
            if (totalReceived > 0) {
                percent = 1.0f - static_cast<float>(lostPackets)/static_cast<float>(totalReceived);
                percent *= 100.0f;
                ImGui::Text("Percent of packets received: %.2f%%", percent);
            }else {
                ImGui::Text("Percent of packets received: %.2f%%", percent);
            }
            
            ImGui::Checkbox("Enable logging everytime a packet is lost", &loggingEnabled);
            if (loggingEnabled) {
                if (lostPacketCount != ComTask::getLostPacketCount()) {
                    GCS_APP_LOG_INFO("Lost packet count: {}", lostPacketCount);
                    lostPacketCount = ComTask::getLostPacketCount();
                }
            }
        }
        
        ImGui::Spacing();
        if (ComTask::getTotalReceivedPackets() > ComTask::getLostPacketCount() && ComTask::getTotalReceivedPackets() > 0) {
            ImGui::Text("Latest Packet Information:");
            
            uint8_t deviceID = GSDataCenter::deviceID;
            uint8_t deviceState = GSDataCenter::deviceState;
            uint64_t latestPacketTime = GSDataCenter::lastPacketReceivedTimestamp_ms;
            
            ImGui::Text("Device ID: %d", deviceID);
            ImGui::Text("Device State: %d", deviceState);
            ImGui::Text("Latest Packet Time: %llu", latestPacketTime);
        }
        
    }
}
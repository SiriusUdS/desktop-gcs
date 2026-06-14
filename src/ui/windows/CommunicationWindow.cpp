#include "CommunicationWindow.h"
#include "ComTask.h"
#include "CommandControl.h"
#include "GSDataCenter.h"
#include "ICom.h"
#include "ITileLoader.h"
#include "Logging.h"
#include "UdpConfig.h"

#include "devices/valve/valve_state.hpp"
#include "peripherals/thermocouple/thermocouple_state.hpp"
#include "system/state.hpp"

#include <array>
#include <cstdio>
#include <cstring>

namespace {
const char* boardStateName(uint8_t rawState) {
    using logic::control::State;
    switch (static_cast<State>(rawState)) {
    case State::Init:   return "INIT";
    case State::Safe:   return "SAFE";
    case State::Unsafe: return "UNSAFE";
    case State::Abort:  return "ABORT";
    case State::Error:  return "ERROR";
    case State::Ignite: return "IGNITE";
    case State::Launch: return "LAUNCH";
    case State::Test:   return "TEST";
    }
    return "Unknown";
}

const char* valveStateName(uint8_t rawState) {
    switch (static_cast<ValveState>(rawState)) {
    case ValveState::Unknown:  return "Unknown";
    case ValveState::Opened:   return "Opened";
    case ValveState::Closed:   return "Closed";
    case ValveState::Opening:  return "Opening";
    case ValveState::Closing:  return "Closing";
    case ValveState::Faulted:  return "Faulted";
    case ValveState::Floating: return "Floating";
    }
    return "Unknown";
}

const char* thermocoupleStateName(uint8_t rawState) {
    switch (static_cast<ThermocoupleState>(rawState)) {
    case ThermocoupleState::Unknown: return "Unknown";
    case ThermocoupleState::Active:  return "Active";
    case ThermocoupleState::Faulted: return "Faulted";
    }
    return "Unknown";
}

// Dense per-board status: header line + valve table + averaged ADC channels (+ FCU thermocouples).
void renderBoardSection(const char* boardName, uint8_t state, uint16_t storageErr, uint32_t controlFlags,
                        const char* valve1Name, const ValveData& valve1,
                        const char* valve2Name, const ValveData& valve2,
                        const std::array<float, AdcChannelAverager::CHANNELS>& adc,
                        const std::atomic<float>* tcTemps, const std::atomic<uint8_t>* tcStates, size_t tcCount) {
    ImGui::PushID(boardName);
    ImGui::SeparatorText(boardName);
    ImGui::Text("State: %s   |   Storage Err: %u   |   Control Flags: 0x%08X",
                boardStateName(state), static_cast<unsigned>(storageErr), static_cast<unsigned>(controlFlags));

    constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit;

    if (ImGui::BeginTable("valves", 6, tableFlags)) {
        ImGui::TableSetupColumn("Valve");
        ImGui::TableSetupColumn("State");
        ImGui::TableSetupColumn("Idle");
        ImGui::TableSetupColumn("Closed Sw");
        ImGui::TableSetupColumn("Open Sw");
        ImGui::TableSetupColumn("Set %");
        ImGui::TableHeadersRow();

        const ValveData* valves[2] = {&valve1, &valve2};
        const char* names[2] = {valve1Name, valve2Name};
        for (int i = 0; i < 2; i++) {
            const ValveData& v = *valves[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::TextUnformatted(names[i]);
            ImGui::TableNextColumn(); ImGui::TextUnformatted(valveStateName(v.state.load()));
            ImGui::TableNextColumn(); ImGui::TextUnformatted(v.isIdle.load() ? "yes" : "no");
            ImGui::TableNextColumn(); ImGui::TextUnformatted(v.closedSwitchHigh.load() ? "1" : "0");
            ImGui::TableNextColumn(); ImGui::TextUnformatted(v.openedSwitchHigh.load() ? "1" : "0");
            ImGui::TableNextColumn(); ImGui::Text("%u", static_cast<unsigned>(v.positionOpened_pct.load()));
        }
        ImGui::EndTable();
    }

    if (ImGui::BeginTable("adc", static_cast<int>(AdcChannelAverager::CHANNELS), tableFlags)) {
        for (size_t i = 0; i < AdcChannelAverager::CHANNELS; i++) {
            char hdr[8];
            std::snprintf(hdr, sizeof(hdr), "Ch%u", static_cast<unsigned>(i));
            ImGui::TableSetupColumn(hdr);
        }
        ImGui::TableHeadersRow();
        ImGui::TableNextRow();
        for (size_t i = 0; i < AdcChannelAverager::CHANNELS; i++) {
            ImGui::TableNextColumn();
            ImGui::Text("%.0f", adc[i]);
        }
        ImGui::EndTable();
    }
    ImGui::TextDisabled("ADC averaged over %u samples/window", static_cast<unsigned>(AdcChannelAverager::WINDOW));

    if (tcTemps != nullptr && tcStates != nullptr && tcCount > 0) {
        if (ImGui::BeginTable("thermocouples", 3, tableFlags)) {
            ImGui::TableSetupColumn("Thermocouple");
            ImGui::TableSetupColumn("Temp (C)");
            ImGui::TableSetupColumn("State");
            ImGui::TableHeadersRow();
            for (size_t i = 0; i < tcCount; i++) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn(); ImGui::Text("TC%u", static_cast<unsigned>(i + 1));
                ImGui::TableNextColumn(); ImGui::Text("%.2f", tcTemps[i].load());
                ImGui::TableNextColumn(); ImGui::TextUnformatted(thermocoupleStateName(tcStates[i].load()));
            }
            ImGui::EndTable();
        }
    }
    ImGui::PopID();
}

// One valve's command row: discrete position buttons that enqueue a SetValvePosition.
void renderValveCommandRow(const char* name, CommandType valveCmd) {
    ImGui::PushID(name);
    ImGui::Text("%-5s", name);
    ImGui::SameLine();
    if (ImGui::Button("Closed")) { CommandControl::sendCommand(valveCmd, 0); }
    ImGui::SameLine();
    if (ImGui::Button("25%")) { CommandControl::sendCommand(valveCmd, 25); }
    ImGui::SameLine();
    if (ImGui::Button("50%")) { CommandControl::sendCommand(valveCmd, 50); }
    ImGui::SameLine();
    if (ImGui::Button("75%")) { CommandControl::sendCommand(valveCmd, 75); }
    ImGui::SameLine();
    if (ImGui::Button("Open")) { CommandControl::sendCommand(valveCmd, 100); }
    ImGui::PopID();
}
} // namespace

const char* const CommunicationWindow::name = "Communication";


const char* CommunicationWindow::getName() const {
    return "Communication";
}

void CommunicationWindow::renderImpl() {
    static bool loggingEnabled = false;
    static uint64_t lostPacketCount = 0;

    if (ImGui::CollapsingHeader("Board Status (averaged)", ImGuiTreeNodeFlags_DefaultOpen)) {
        const std::array<float, AdcChannelAverager::CHANNELS> motorAdc = GSDataCenter::motorAdcAverager.latestAverage();
        const std::array<float, AdcChannelAverager::CHANNELS> fcuAdc = GSDataCenter::fillingStationAdcAverager.latestAverage();

        renderBoardSection("Motor (ECU)",
                           GSDataCenter::motorBoardState.load(),
                           GSDataCenter::motorBoardStorageErrorStatus.load(),
                           GSDataCenter::motorBoardControlFlags.load(),
                           "NOS", GSDataCenter::nosValveData,
                           "IPA", GSDataCenter::ipaValveData,
                           motorAdc, nullptr, nullptr, 0);

        renderBoardSection("Filling Station (FCU)",
                           GSDataCenter::fillingStationBoardState.load(),
                           GSDataCenter::fillingStationBoardStorageErrorStatus.load(),
                           GSDataCenter::fillingStationBoardControlFlags.load(),
                           "Fill", GSDataCenter::fillValveData,
                           "Dump", GSDataCenter::dumpValveData,
                           fcuAdc,
                           GSDataCenter::fillingStationThermocouple_C,
                           GSDataCenter::fillingStationThermocoupleState,
                           GSDataCenterConfig::THERMOCOUPLE_AMOUNT);
    }

    if (ImGui::CollapsingHeader("Commands")) {
        ImGui::TextUnformatted("Valve position");
        renderValveCommandRow("NOS", CommandType::NosValve);
        renderValveCommandRow("IPA", CommandType::IpaValve);
        renderValveCommandRow("Fill", CommandType::FillValve);
        renderValveCommandRow("Dump", CommandType::DumpValve);

        ImGui::Separator();
        if (ImGui::Button("Ping")) {
            CommandControl::sendCommand(CommandType::Ping, 0);
        }
        ImGui::SameLine();
        ImGui::Text("Pongs received: %u  (last from board %u)",
                    static_cast<unsigned>(GSDataCenter::pongReceivedCount.load()),
                    static_cast<unsigned>(GSDataCenter::lastPongSenderId.load()));
    }

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
        
    }
}
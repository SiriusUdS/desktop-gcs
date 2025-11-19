#include "SerialComWindow.h"

#include "BoardComStateMonitor.h"
#include "ComPortSelector.h"
#include "FontConfig.h"
#include "GSDataCenter.h"
#include "IniConfig.h"
#include "PacketRateMonitor.h"
#include "SerialCom.h"
#include "SerialConfig.h"
#include "SerialTask.h"
#include "Telecommunication/BoardCommand.h"

#include <algorithm>
#include <imgui.h>

namespace SerialComWindow {
enum RecvBufferDisplayMode { TEXT = 0, HEXA = 1 };

constexpr const char* INI_RECV_BUFFER_DISPLAY_MODE = "recv_buffer_display_mode";

void renderBoardComStateTableRow(const char* boardName, BoardComStateMonitor::State state);
void renderPacketRateTableRow(const char* packetName, double rate);
void recvBufferContentModal();
void updateRecvBufferContentDisplay(bool syncToCurrentBuffer);

int recvBufferDisplayMode{TEXT};
bool recvBufferDisplayPause{};
std::vector<char> recvBufferContentDisplay(SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE);
} // namespace SerialComWindow

void SerialComWindow::render() {
    if (ImGui::CollapsingHeader("Board COM")) {
        std::string comPortStr = SerialTask::comPortSelector.available() ? SerialTask::comPortSelector.current() : "None available";
        ImGui::Text("COM port: %s", comPortStr.c_str());

        if (ImGui::BeginTable("BoardComStatesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Board");
            ImGui::TableSetupColumn("COM State");
            ImGui::TableHeadersRow();

            renderBoardComStateTableRow("Motor", SerialTask::motorBoardComStateMonitor.getState());
            renderBoardComStateTableRow("Filling Station", SerialTask::fillingStationBoardComStateMonitor.getState());
            renderBoardComStateTableRow("GS Control", SerialTask::gsControlBoardComStateMonitor.getState());
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Packet rates")) {
        if (ImGui::BeginTable("PacketRatesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Packet Type");
            ImGui::TableSetupColumn("Packets/s");
            ImGui::TableHeadersRow();

            renderPacketRateTableRow("Engine telemetry", SerialTask::engineTelemetryPacketRateMonitor.getRatePerSecond());
            renderPacketRateTableRow("Filling station telemetry", SerialTask::fillingStationTelemetryPacketRateMonitor.getRatePerSecond());
            renderPacketRateTableRow("GS control", SerialTask::gsControlPacketRateMonitor.getRatePerSecond());
            renderPacketRateTableRow("Engine status", SerialTask::engineStatusPacketRateMonitor.getRatePerSecond());
            renderPacketRateTableRow("Filling station status", SerialTask::fillingStationStatusPacketRateMonitor.getRatePerSecond());
            ImGui::PushFont(FontConfig::boldMainFont);
            renderPacketRateTableRow("Total", SerialTask::packetRateMonitor.getRatePerSecond());
            ImGui::PopFont();

            ImGui::EndTable();
        }
    }

    std::string lastReceivedMotorBoardCommandName = "UNKNOWN";
    switch (GSDataCenter::lastReceivedCommandCodeMotorBoard) {
    case BOARD_COMMAND_CODE_ACK:
        lastReceivedMotorBoardCommandName = "ACK";
        break;
    case BOARD_COMMAND_CODE_UNSAFE:
        lastReceivedMotorBoardCommandName = "UNSAFE";
        break;
    case BOARD_COMMAND_CODE_SAFE:
        lastReceivedMotorBoardCommandName = "SAFE";
        break;
    case BOARD_COMMAND_CODE_ABORT:
        lastReceivedMotorBoardCommandName = "ABORT";
        break;
    case BOARD_COMMAND_CODE_RESET:
        lastReceivedMotorBoardCommandName = "RESET";
        break;
    case ENGINE_COMMAND_CODE_OPEN_VALVE:
        lastReceivedMotorBoardCommandName = "OPEN VALVE";
        break;
    case ENGINE_COMMAND_CODE_CLOSE_VALVE:
        lastReceivedMotorBoardCommandName = "CLOSE VALVE";
        break;
    case ENGINE_COMMAND_CODE_FIRE_IGNITER:
        lastReceivedMotorBoardCommandName = "FIRE IGNITER";
        break;
    case ENGINE_COMMAND_CODE_SET_NOS_VALVE_HEATER_POWER_PCT:
        lastReceivedMotorBoardCommandName = "SET NOS VALVE HEATER POWER PCT";
        break;
    case ENGINE_COMMAND_CODE_SET_IPA_VALVE_HEATER_POWER_PCT:
        lastReceivedMotorBoardCommandName = "SET IPA VALVE HEATER POWER PCT";
        break;
    }

    std::string lastReceivedFillingStationBoardCommandName = "UNKNOWN";
    switch (GSDataCenter::lastReceivedCommandCodeFillingStationBoard) {
    case BOARD_COMMAND_CODE_ACK:
        lastReceivedFillingStationBoardCommandName = "ACK";
        break;
    case BOARD_COMMAND_CODE_UNSAFE:
        lastReceivedFillingStationBoardCommandName = "UNSAFE";
        break;
    case BOARD_COMMAND_CODE_SAFE:
        lastReceivedFillingStationBoardCommandName = "SAFE";
        break;
    case BOARD_COMMAND_CODE_ABORT:
        lastReceivedFillingStationBoardCommandName = "ABORT";
        break;
    case BOARD_COMMAND_CODE_RESET:
        lastReceivedFillingStationBoardCommandName = "RESET";
        break;
    case FILLING_STATION_COMMAND_CODE_OPEN_FILL_VALVE_PCT:
        lastReceivedFillingStationBoardCommandName = "OPEN FILL VALVE PCT";
        break;
    case FILLING_STATION_COMMAND_CODE_OPEN_DUMP_VALVE_PCT:
        lastReceivedFillingStationBoardCommandName = "OPEN DUMP VALVE PCT";
        break;
    case FILLING_STATION_COMMAND_CODE_SET_FILL_VALVE_HEATER_POWER_PCT:
        lastReceivedFillingStationBoardCommandName = "SET FILL VALVE HEATER POWER PCT";
        break;
    case FILLING_STATION_COMMAND_CODE_SET_DUMP_VALVE_HEATER_POWER_PCT:
        lastReceivedFillingStationBoardCommandName = "SET DUMP VALVE HEATER POWER PCT";
        break;
    }

    std::string lastBoardSentCommandName = "UNKNOWN";
    switch (GSDataCenter::lastBoardSentCommandCode) {
    case FILLING_STATION_COMMAND_CODE_OPEN_FILL_VALVE_PCT:
        lastBoardSentCommandName = "OPEN FILL VALVE PCT";
        break;
    case FILLING_STATION_COMMAND_CODE_OPEN_DUMP_VALVE_PCT:
        lastBoardSentCommandName = "OPEN DUMP VALVE PCT";
        break;
    case ENGINE_COMMAND_CODE_SET_NOS_VALVE_HEATER_POWER_PCT:
        lastBoardSentCommandName = "SET NOS/FILL VALVE HEATER POWER PCT";
        break;
    case ENGINE_COMMAND_CODE_SET_IPA_VALVE_HEATER_POWER_PCT:
        lastBoardSentCommandName = "SET IPA/DUMP VALVE HEATER POWER PCT";
        break;
    case BOARD_COMMAND_CODE_ABORT:
        lastBoardSentCommandName = "ABORT";
        break;
    case BOARD_COMMAND_CODE_RESET:
        lastBoardSentCommandName = "RESET";
        break;
    }

    if (ImGui::CollapsingHeader("Misc")) {
        ImGui::PushFont(FontConfig::boldMainFont);
        ImGui::Text("Launch steps");
        ImGui::PopFont();

        ImGui::Text("Ignite timestamp (ms): %d", GSDataCenter::igniteTimestamp_ms);
        ImGui::Text("Launch timestamp (ms): %d", GSDataCenter::launchTimestamp_ms);

        ImGui::Separator();

        ImGui::PushFont(FontConfig::boldMainFont);
        ImGui::Text("Last commands");
        ImGui::PopFont();

        ImGui::Text("Last command received by motor board: %s", lastReceivedMotorBoardCommandName.c_str());
        ImGui::Text("Last command received by filling station board: %s", lastReceivedFillingStationBoardCommandName.c_str());

        ImGui::Text("Last command GS sent to boards: %s", lastBoardSentCommandName.c_str());

        ImGui::Text("Time since last command received by motor board (ms): %d", GSDataCenter::timeSinceLastCommandMotorBoard_ms);
        ImGui::Text("Time since last command received by filling station board (ms): %d", GSDataCenter::timeSinceLastCommandFillingStationBoard_ms);

        // ImGui::Text("Last command sent to GS timestamp (ms): %d", GSDataCenter::lastReceivedGSCommandTimestamp_ms);
        // ImGui::Text("Last GCS command received timestamp (ms): %d", GSDataCenter::lastSentCommandTimestamp_ms);
    }

    if (ImGui::CollapsingHeader("RECV Buffer")) {
        if (ImGui::Button("View RECV buffer content")) {
            ImGui::OpenPopup("RECV Buffer Content");
        }
    }

    recvBufferContentModal();
}

void SerialComWindow::loadState(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        if (ini.get(IniConfig::GCS_SECTION).has(INI_RECV_BUFFER_DISPLAY_MODE)) {
            recvBufferDisplayMode = std::stoi(ini.get(IniConfig::GCS_SECTION).get(INI_RECV_BUFFER_DISPLAY_MODE));
        }
    }
}

void SerialComWindow::saveState(mINI::INIStructure& ini) {
    ini[IniConfig::GCS_SECTION].set(INI_RECV_BUFFER_DISPLAY_MODE, std::to_string(recvBufferDisplayMode));
}

void SerialComWindow::renderBoardComStateTableRow(const char* boardName, BoardComStateMonitor::State state) {
    const char* comStateText = "Unknown";
    if (!SerialTask::com.comOpened()) {
        comStateText = "Disconnected";
    } else {
        switch (state) {
        case BoardComStateMonitor::State::STARTING:
            comStateText = "Starting";
            break;
        case BoardComStateMonitor::State::WORKING:
            comStateText = "Working";
            break;
        case BoardComStateMonitor::State::NOT_WORKING:
            comStateText = "Not working";
            break;
        }
    }

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(boardName);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(comStateText);
}

void SerialComWindow::renderPacketRateTableRow(const char* packetName, double rate) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(packetName);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%.1f", rate);
}

void SerialComWindow::recvBufferContentModal() {
    updateRecvBufferContentDisplay(!recvBufferDisplayPause);

    const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    const ImVec2 minBoxSize = {400, 200};
    const ImVec2 maxBoxSize = {2000, 1200};
    const ImVec2 boxSize = {std::clamp(screenSize.x * 0.8f, minBoxSize.x, maxBoxSize.x), std::clamp(screenSize.y * 0.6f, minBoxSize.y, maxBoxSize.y)};
    const ImVec2 boxPos = {(screenSize.x - boxSize.x) * 0.5f - 10.0f, (screenSize.y - boxSize.y) * 0.5f - 60.0f};

    ImGui::SetNextWindowPos(boxPos, ImGuiCond_Always);

    const ImGuiWindowFlags modalFlags =
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove;

    if (ImGui::BeginPopupModal("RECV Buffer Content", nullptr, modalFlags)) {
        ImGui::RadioButton("Text", &recvBufferDisplayMode, TEXT);
        ImGui::SameLine();
        ImGui::RadioButton("Hexa", &recvBufferDisplayMode, HEXA);
        ImGui::SameLine();
        ImGui::Checkbox("Pause", &recvBufferDisplayPause);

        const char* displayBuf = recvBufferContentDisplay.data();

        ImGui::BeginChild("RecvBufferBox", boxSize, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::PushTextWrapPos(0.0f);
        ImGui::PushFont(FontConfig::monospaceFont);

        if (recvBufferDisplayMode == TEXT) {
            ImGui::TextUnformatted(displayBuf, displayBuf + SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE);
        } else {
            ImGui::BeginGroup();
            for (size_t i = 0; i < SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE; ++i) {
                unsigned char byte = static_cast<unsigned char>(displayBuf[i]);

                bool dim = byte == '\0';
                if (dim) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
                }

                char hexByte[4]; // 2 hex digits + space + null terminator
                std::snprintf(hexByte, sizeof(hexByte), "%02X ", byte);
                ImGui::TextUnformatted(hexByte);

                if (dim) {
                    ImGui::PopStyleColor();
                }

                if ((i + 1) % 16 != 0) {
                    ImGui::SameLine();
                }
            }
            ImGui::EndGroup();
        }

        ImGui::PopFont();
        ImGui::PopTextWrapPos();
        ImGui::EndChild();

        if (ImGui::Button("Close", {boxSize.x, 0})) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SerialComWindow::updateRecvBufferContentDisplay(bool syncToCurrentBuffer) {
    const char* buf = (char*) SerialTask::com.getBuffer();
    const char zeroChar = recvBufferDisplayMode == TEXT ? '~' : '\0';

    if (syncToCurrentBuffer) {
        // Sync recv buffer display
        for (size_t i = 0; i < SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE; i++) {
            if (buf[i] == '\0') {
                recvBufferContentDisplay[i] = zeroChar;
            } else {
                recvBufferContentDisplay[i] = buf[i];
            }
        }
    } else {
        // Only modify how zeros are represented
        const char charToReplace = recvBufferDisplayMode == TEXT ? '\0' : '~';
        for (size_t i = 0; i < SerialConfig::PACKET_CIRCULAR_BUFFER_SIZE; i++) {
            if (recvBufferContentDisplay[i] == charToReplace) {
                recvBufferContentDisplay[i] = zeroChar;
            }
        }
    }
}

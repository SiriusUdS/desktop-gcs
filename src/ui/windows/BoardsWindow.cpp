#include "BoardsWindow.h"

#include "BoardComStateMonitor.h"
#include "Engine/EngineState.h"
#include "FillingStation/FillingStationState.h"
#include "FontConfig.h"
#include "GSControl/GSControlState.h"
#include "GSDataCenter.h"
#include "SerialCom.h"
#include "SerialTask.h"
#include "Storage/StorageErrorStatus.h"

#include <imgui.h>

namespace BoardsWindow {
void renderBoardTableRow(const char* name, const char* boardStateName, BoardComStateMonitor::State comState);
void renderStorageErrorStatusName(uint16_t storageErrorStatus);
} // namespace BoardsWindow

void BoardsWindow::render() {
    if (ImGui::CollapsingHeader("State")) {
        const char* motorBoardStateName = "Unknown";
        switch (GSDataCenter::motorBoardState) {
        case ENGINE_STATE_INIT:
            motorBoardStateName = "INIT";
            break;
        case ENGINE_STATE_SAFE:
            motorBoardStateName = "SAFE";
            break;
        case ENGINE_STATE_UNSAFE:
            motorBoardStateName = "UNSAFE";
            break;
        case ENGINE_STATE_ABORT:
            motorBoardStateName = "ABORT";
            break;
        case ENGINE_STATE_IGNITION:
            motorBoardStateName = "IGNITION";
            break;
        case ENGINE_STATE_LAUNCH:
            motorBoardStateName = "LAUNCH";
            break;
        }

        const char* fillingStationBoardStateName = "Unknown";
        switch (GSDataCenter::fillingStationBoardState) {
        case FILLING_STATION_STATE_INIT:
            fillingStationBoardStateName = "INIT";
            break;
        case FILLING_STATION_STATE_SAFE:
            fillingStationBoardStateName = "SAFE";
            break;
        case FILLING_STATION_STATE_UNSAFE:
            fillingStationBoardStateName = "UNSAFE";
            break;
        case FILLING_STATION_STATE_ABORT:
            fillingStationBoardStateName = "ABORT";
            break;
        }

        const char* gsControlBoardStateName = "Unknown";
        switch (GSDataCenter::gsControlBoardState) {
        case GS_CONTROL_STATE_INIT:
            gsControlBoardStateName = "INIT";
            break;
        case GS_CONTROL_STATE_SAFE:
            gsControlBoardStateName = "SAFE";
            break;
        case GS_CONTROL_STATE_UNSAFE:
            gsControlBoardStateName = "UNSAFE";
            break;
        case GS_CONTROL_STATE_ABORT:
            gsControlBoardStateName = "ABORT";
            break;
        }

        if (ImGui::BeginTable("BoardComStatesTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Board");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("COM State");
            ImGui::TableHeadersRow();

            renderBoardTableRow("Motor", motorBoardStateName, SerialTask::motorBoardComStateMonitor.getState());
            renderBoardTableRow("Filling Station", fillingStationBoardStateName, SerialTask::fillingStationBoardComStateMonitor.getState());
            renderBoardTableRow("GS Control", gsControlBoardStateName, SerialTask::gsControlBoardComStateMonitor.getState());
            ImGui::EndTable();
        }
    }

    if (ImGui::CollapsingHeader("Storage Error Status")) {
        ImGui::PushFont(FontConfig::boldMainFont);
        ImGui::Text("Any value other than 0 indicates an error.");
        ImGui::PopFont();

        ImGui::Text("Motor Board: ");
        ImGui::SameLine();
        renderStorageErrorStatusName(GSDataCenter::motorBoardStorageErrorStatus);

        ImGui::Text("Filling Station Board: ");
        ImGui::SameLine();
        renderStorageErrorStatusName(GSDataCenter::fillingStationBoardStorageErrorStatus);
    }
}

void BoardsWindow::renderBoardTableRow(const char* name, const char* boardStateName, BoardComStateMonitor::State comState) {
    const char* comStateText = "Unknown";
    if (!SerialTask::com.comOpened()) {
        comStateText = "Disconnected";
    } else {
        switch (comState) {
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
    ImGui::Text(name);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text(boardStateName);
    ImGui::TableSetColumnIndex(2);
    ImGui::Text(comStateText);
}

void BoardsWindow::renderStorageErrorStatusName(uint16_t storageErrorStatus) {
    ImGui::Text("%d", storageErrorStatus);
}

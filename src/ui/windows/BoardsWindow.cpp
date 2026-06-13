#include "BoardsWindow.h"

#include "ComTask.h"
#include "FontConfig.h"
#include "GSDataCenter.h"
#include "UdpCom.h"

#include "system/state.hpp"

#include <imgui.h>

namespace {
/// Map a board's raw wire state byte to a display name via the shared
/// logic::control::State enum (common-protocol's single source of truth; all
/// boards now share one state encoding instead of per-board *_STATE_* macros).
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
} // namespace

const char* BoardsWindow::getName() const {
    return "Boards";
}

void BoardsWindow::renderImpl() {
    if (ImGui::CollapsingHeader("State")) {
        const char* motorBoardStateName = boardStateName(GSDataCenter::motorBoardState);
        const char* fillingStationBoardStateName = boardStateName(GSDataCenter::fillingStationBoardState);
        const char* gsControlBoardStateName = boardStateName(GSDataCenter::gsControlBoardState);

        if (ImGui::BeginTable("BoardComStatesTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Board");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("COM State");
            ImGui::TableHeadersRow();

            renderBoardTableRow("Motor", motorBoardStateName, ComTask::motorBoardComStateMonitor.getState());
            renderBoardTableRow("Filling Station", fillingStationBoardStateName, ComTask::fillingStationBoardComStateMonitor.getState());
            renderBoardTableRow("GS Control", gsControlBoardStateName, ComTask::gsControlBoardComStateMonitor.getState());
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

void BoardsWindow::renderBoardTableRow(const char* name, const char* boardStateName, BoardComStateMonitor::State comState) const {
    const char* comStateText = "Unknown";
    if (!ComTask::com->comOpened()) {
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
    ImGui::Text("%s", name);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", boardStateName);
    ImGui::TableSetColumnIndex(2);
    ImGui::Text("%s", comStateText);
}

void BoardsWindow::renderStorageErrorStatusName(uint16_t storageErrorStatus) const {
    ImGui::Text("%d", storageErrorStatus);
}

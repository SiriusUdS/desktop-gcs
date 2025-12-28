#include "ValvesWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "ValveData.h"

#include <imgui.h>

const char* ValvesWindow::getName() const {
    return "Valves";
}

const char* ValvesWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void ValvesWindow::renderImpl() {
    if (ImGui::BeginTable("ValveStateTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Valve");
        ImGui::TableSetupColumn("Is idle");
        ImGui::TableSetupColumn("Closed switch");
        ImGui::TableSetupColumn("Opened switch");
        ImGui::TableSetupColumn("% Open");
        ImGui::TableHeadersRow();

        renderValveStateRow("IPA", GSDataCenter::nosValveData);
        renderValveStateRow("NOS", GSDataCenter::ipaValveData);
        renderValveStateRow("Dump", GSDataCenter::dumpValveData);
        renderValveStateRow("Fill", GSDataCenter::fillValveData);

        ImGui::EndTable();
    }
}

void ValvesWindow::renderValveStateRow(const char* label, ValveData& data) const {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", label);

    ImGui::TableSetColumnIndex(1);
    ImGui::Text(data.isIdle ? "Yes" : "No");

    ImGui::TableSetColumnIndex(2);
    ImGui::Text(data.closedSwitchHigh ? "Unpressed"
                                      : "Pressed"); // TODO: This is a temp hotfix for LC25, on and off are switched temporarily because elec

    ImGui::TableSetColumnIndex(3);
    ImGui::Text(data.openedSwitchHigh ? "Unpressed"
                                      : "Pressed"); // TODO: This is a temp hotfix for LC25, on and off are switched temporarily because elec

    ImGui::TableSetColumnIndex(4);
    ImGui::Text("%d", data.positionOpened_pct.load());
}

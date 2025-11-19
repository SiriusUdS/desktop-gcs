#include "SwitchesWindow.h"

#include "GSDataCenter.h"
#include "OnOff.h"
#include "SwitchData.h"
#include "UIConfig.h"

#include <imgui.h>
#include <string>

namespace SwitchesWindow {
void renderSwitch(SwitchData& data, const char* name);
}

void SwitchesWindow::render() {
    renderSwitch(GSDataCenter::AllowDumpSwitchData, "Allow Dump");
    renderSwitch(GSDataCenter::AllowFillSwitchData, "Allow Fill");
    renderSwitch(GSDataCenter::ArmIgniterSwitchData, "Arm Igniter");
    renderSwitch(GSDataCenter::ArmServoSwitchData, "Arm Servo");
    renderSwitch(GSDataCenter::EmergencyStopButtonData, "Emergency Stop");
    renderSwitch(GSDataCenter::FireIgniterButtonData, "Fire Igniter");
    renderSwitch(GSDataCenter::ValveStartButtonData, "Valve Start");
}

void SwitchesWindow::renderSwitch(SwitchData& data, const char* name) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", name);
    ImGui::SameLine(260);
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    OnOff(data.isOn, name);
    ImGui::PopStyleColor();
}

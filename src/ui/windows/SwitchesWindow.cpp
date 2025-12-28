#include "SwitchesWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "OnOff.h"

#include <imgui.h>

const char* SwitchesWindow::getName() const {
    return "Switches";
}

const char* SwitchesWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void SwitchesWindow::renderImpl() {
    renderSwitch(GSDataCenter::AllowDumpSwitchData, "Allow Dump");
    renderSwitch(GSDataCenter::AllowFillSwitchData, "Allow Fill");
    renderSwitch(GSDataCenter::ArmIgniterSwitchData, "Arm Igniter");
    renderSwitch(GSDataCenter::ArmServoSwitchData, "Arm Servo");
    renderSwitch(GSDataCenter::EmergencyStopButtonData, "Emergency Stop");
    renderSwitch(GSDataCenter::FireIgniterButtonData, "Fire Igniter");
    renderSwitch(GSDataCenter::ValveStartButtonData, "Valve Start");
}

void SwitchesWindow::renderSwitch(const SwitchData& data, const char* name) const {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", name);
    ImGui::SameLine(260);
    OnOff(data.isOn, name);
}

#include "OnOff.h"

#include "UIConfig.h"

#include <imgui.h>
#include <string>

void OnOff(bool isOn, const char* id) {
    std::string label = isOn ? "ON###" : "OFF###";
    label += id;
    ImVec4 color = isOn ? UIConfig::GREEN_BUTTON_COLOR : UIConfig::RED_BUTTON_COLOR;

    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

    ImGui::Button(label.c_str(), ImVec2(180, 0));

    ImGui::PopStyleColor(3);
}

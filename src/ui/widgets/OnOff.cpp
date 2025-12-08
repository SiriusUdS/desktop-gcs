#include "OnOff.h"

#include "ThemedColors.h"

#include <imgui.h>
#include <string>

void OnOff(bool isOn, const char* id) {
    std::string label = isOn ? "ON###" : "OFF###";
    label += id;
    ImVec4 color = isOn ? ThemedColors::Button::green.resolve() : ThemedColors::Button::red.resolve();

    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

    ImGui::Button(label.c_str(), ImVec2(180, 0));

    ImGui::PopStyleColor(3);
}

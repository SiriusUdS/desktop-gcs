#include "ToggleButton.h"

#include "WidgetUtils.h"

bool ToggleButton(const char* label, bool* state, ImVec4 baseColor) {
    float brightnessFactor = *state ? 1.0f : 0.6f;
    ImVec4 mainColor = AdjustColor(baseColor, brightnessFactor);
    ImVec4 hoverColor = AdjustColor(mainColor, 1.1f);
    ImVec4 activeColor = AdjustColor(mainColor, 0.9f);

    ImGui::PushStyleColor(ImGuiCol_Button, mainColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);

    bool clicked = ImGui::Button(label);
    if (clicked) {
        *state = !*state;
    }

    ImGui::PopStyleColor(3);
    return clicked;
}

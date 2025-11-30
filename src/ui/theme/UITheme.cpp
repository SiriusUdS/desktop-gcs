#include "UITheme.h"

#include <imgui.h>

namespace UITheme {
bool cachedIsDarkTheme = false;

void update() {
    const ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    cachedIsDarkTheme = textColor.x > 0.5f && textColor.y > 0.5f && textColor.z > 0.5f;
}

bool isDarkTheme() {
    return cachedIsDarkTheme;
}
} // namespace UITheme

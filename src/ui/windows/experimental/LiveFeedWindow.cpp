#include "LiveFeedWindow.h"

#include <imgui.h>

const char* LiveFeedWindow::getName() const {
    return "Live Feed";
}

void LiveFeedWindow::renderImpl() {
    ImGui::Text("TODO: Live feed");
}

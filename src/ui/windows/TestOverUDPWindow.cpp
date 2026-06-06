#include "TestOverUDPWindow.h"

#include "UdpTestCommands.h"

#include <imgui.h>

TestOverUDPWindow::TestOverUDPWindow() {
    
}

const char* TestOverUDPWindow::getName() const {
    return "Test Over UDP";
}

void TestOverUDPWindow::renderImpl() {
    ImGui::Text("Send command to valves");
    static int valveValue = 0;
    ImGui::InputInt("Valve value", &valveValue);
    if (ImGui::Button("Test")) {
        UdpTestCommands::testValve(static_cast<uint16_t>(valveValue));
    }
}
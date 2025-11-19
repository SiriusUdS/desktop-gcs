#include "ControlsWindow.h"

#include "CommandControl.h"
#include "CommandTypes.h"
#include "Engine/EngineSensors.h"
#include "Engine/EngineState.h"
#include "FillingStation/FillingStationSensors.h"
#include "GSDataCenter.h"
#include "Logging.h"
#include "SwitchData.h"
#include "ValveData.h"

#include <imgui.h>

namespace ControlsWindow {
/**
 * @struct ValveSlider
 * @brief Holds the state of a valve slider.
 */
struct PercentageInput {
    int openedValue_perc{};        ///< Value in percentage of the input.
    int lastSetOpenedValue_perc{}; ///< Value in percentage of the input that was last sent.
    int lastOpenedValue_perc{};    ///< Last value sent to the command control.
};

PercentageInput nosValveSlider;
PercentageInput ipaValveSlider;
PercentageInput fillValveSlider;
PercentageInput dumpValveSlider;
PercentageInput nosHeatPadSlider;
PercentageInput ipaHeatPadSlider;
PercentageInput fillHeatPadSlider;
PercentageInput dumpHeatPadSlider;

static void renderPercentageInput(const char* name,
                                  PercentageInput& slider,
                                  CommandType commandType,
                                  const char* tooltipDisabled = "",
                                  bool inputEnabled = true,
                                  bool onlyFullyClosedOrOpen = false);
} // namespace ControlsWindow

void ControlsWindow::render() {
    if (ImGui::CollapsingHeader("Valves")) {
        const bool nosAndIpaValveSliderEnabled = GSDataCenter::motorBoardState == ENGINE_STATE_UNSAFE && GSDataCenter::ArmServoSwitchData.isOn
                                                 && !GSDataCenter::AllowDumpSwitchData.isOn && !GSDataCenter::AllowFillSwitchData.isOn
                                                 && !GSDataCenter::ArmIgniterSwitchData.isOn;
        const bool fillValveSliderEnabled = GSDataCenter::AllowFillSwitchData.isOn;
        const bool dumpValveSliderEnabled = GSDataCenter::AllowDumpSwitchData.isOn;

        renderPercentageInput("NOS Valve",
                              nosValveSlider,
                              CommandType::NosValve,
                              "To control the NOS valve -> [UNSAFE, ARM VALVE] need to be ON, [DUMP, FILL, ARM IGNITER] need to be OFF.",
                              nosAndIpaValveSliderEnabled);

        renderPercentageInput("IPA Valve",
                              ipaValveSlider,
                              CommandType::IpaValve,
                              "To control the IPA valve -> [UNSAFE, ARM VALVE] need to be ON, [DUMP, FILL, ARM IGNITER] need to be OFF.",
                              nosAndIpaValveSliderEnabled);

        renderPercentageInput("Fill Valve",
                              fillValveSlider,
                              CommandType::FillValve,
                              "To control the FILL valve -> [UNSAFE, FILL] need to be ON.",
                              GSDataCenter::AllowFillSwitchData.isOn);

        renderPercentageInput("Dump Valve",
                              dumpValveSlider,
                              CommandType::DumpValve,
                              "To control the DUMP valve -> [UNSAFE, DUMP] need to be ON.",
                              GSDataCenter::AllowDumpSwitchData.isOn);
    }

    if (ImGui::CollapsingHeader("Solenoid valve")) {
        const bool solenoidValveSliderEnabled = GSDataCenter::motorBoardState == ENGINE_STATE_UNSAFE;
        renderPercentageInput("Solenoid Valve",
                              dumpHeatPadSlider,
                              CommandType::DumpHeatPad,
                              "To control the solenoid valve -> [UNSAFE] needs to be ON.",
                              solenoidValveSliderEnabled,
                              true); // TODO: This is a temp hotfix for LC25
    }

    if (ImGui::CollapsingHeader("Heat pads")) {
        renderPercentageInput("Nos Heat Pad", nosHeatPadSlider, CommandType::NosHeatPad);
        renderPercentageInput("Ipa Heat Pad", ipaHeatPadSlider, CommandType::IpaHeatPad);
        renderPercentageInput("Fill Heat Pad", fillHeatPadSlider, CommandType::FillHeatPad);
    }

    if (ImGui::CollapsingHeader("Reset & Abort")) {
        if (ImGui::Button("Reset")) {
            CommandControl::sendCommand(CommandType::Reset, 0);
        }
        ImGui::SameLine();
        if (ImGui::Button("Abort")) {
            CommandControl::sendCommand(CommandType::Abort, 0);
        }
    }
}

void ControlsWindow::renderPercentageInput(const char* name,
                                           PercentageInput& slider,
                                           CommandType commandType,
                                           const char* tooltipDisabled,
                                           bool inputEnabled,
                                           bool onlyFullyClosedOrOpen) {
    // TODO: Do we implement onlyFullyClosedOrOpen in this function or do we just remove it?

    constexpr float inputWidth = 200.0f;

    ImGui::BeginDisabled(!inputEnabled);

    // Render int input
    ImGui::SetNextItemWidth(inputWidth);
    ImGui::InputInt(name, &slider.openedValue_perc);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !inputEnabled) {
        ImGui::SetTooltip(tooltipDisabled);
    }
    ImGui::SameLine(inputWidth + 200.0f);

    // Prevent out of bound percentage value
    if (slider.openedValue_perc < 0) {
        slider.openedValue_perc = 0;
    } else if (slider.openedValue_perc > 100) {
        slider.openedValue_perc = 100;
    }

    // Current value text
    ImGui::Text("Open: %d%%", slider.lastSetOpenedValue_perc);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !inputEnabled) {
        ImGui::SetTooltip(tooltipDisabled);
    }
    ImGui::SameLine(inputWidth + 375.0f);

    // Button to set the percentage value
    std::string buttonStr = std::string("Set Value") + "##" + name;
    if (ImGui::Button(buttonStr.c_str())) {
        slider.lastSetOpenedValue_perc = slider.openedValue_perc;
        CommandControl::sendCommand(commandType, slider.openedValue_perc);
    }
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !inputEnabled) {
        ImGui::SetTooltip(tooltipDisabled);
    }

    ImGui::EndDisabled();
}

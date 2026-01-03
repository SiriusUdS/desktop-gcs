#include "FillWindow.h"

#include "CommandControl.h"
#include "ConfigParams.h"
#include "Engine/EngineState.h"
#include "GSDataCenter.h"
#include "PrelaunchWindow.h"
#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ThemedColors.h"

#include <imgui.h>
#include <implot.h>

const char* const FillWindow::name = "Fill";

FillWindow::FillWindow()
    : tankLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell", ThemedColors::PlotLine::blue)},
      tankTransducerPlotLine{GSDataCenter::PressureSensor_FillingStation_PlotData.p1().getValuePlotData(), // TODO: Is this correct index?
                             PlotStyle("Tank Pressure", ThemedColors::PlotLine::red)},
      tankTempPlotLine{GSDataCenter::Thermistor_FillingStation_PlotData.t1().getValuePlotData(), // TODO: Is this correct index?
                       PlotStyle("Tank Temperature", ThemedColors::PlotLine::green)},
      tankMassPlotLine{GSDataCenter::NOSTankMass_PlotData, PlotStyle("Tank Mass", ThemedColors::PlotLine::yellow)} {
}

const char* FillWindow::getName() const {
    return name;
}

void FillWindow::renderImpl() {
    const bool nosAndIpaValveSliderEnabled = GSDataCenter::motorBoardState == ENGINE_STATE_UNSAFE && GSDataCenter::ArmServoSwitchData.isOn
                                             && !GSDataCenter::AllowDumpSwitchData.isOn && !GSDataCenter::AllowFillSwitchData.isOn
                                             && !GSDataCenter::ArmIgniterSwitchData.isOn;
    const bool fillValveSliderEnabled = GSDataCenter::AllowFillSwitchData.isOn;
    const bool dumpValveSliderEnabled = GSDataCenter::AllowDumpSwitchData.isOn;
    const bool solenoidValveSliderEnabled = GSDataCenter::motorBoardState == ENGINE_STATE_UNSAFE;

    ImGui::SeparatorText("Tank Data Plot");

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Data", {-1.0f, 800.0f}, ImPlotFlags_NoInputs)) {
        constexpr ImAxis weightAxis = ImAxis_Y1;
        constexpr ImAxis pressureAxis = ImAxis_Y2;
        constexpr ImAxis tempAxis = ImAxis_Y3;

        ImPlot::SetupAxis(ImAxis_X1, "Timestamp (ms)");
        ImPlot::SetupAxis(weightAxis, "Weight (lb)");
        ImPlot::SetupAxis(pressureAxis, "Pressure (psi)");
        ImPlot::SetupAxis(tempAxis, "Temperature (C)");

        ImPlot::SetAxis(weightAxis);
        tankLoadCellPlotLine.plot(ConfigParams::compressPlots.currentValue);
        tankMassPlotLine.plot(ConfigParams::compressPlots.currentValue);

        ImPlot::SetAxis(pressureAxis);
        tankTransducerPlotLine.plot(ConfigParams::compressPlots.currentValue);

        ImPlot::SetAxis(tempAxis);
        tankTempPlotLine.plot(ConfigParams::compressPlots.currentValue);

        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Valve Control");

    if (ImGui::BeginTable("fill_valve_control_table", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Input");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Open Percentage");
        ImGui::TableSetupColumn("Set Value Button");

        renderPercentageInputRow("NOS Valve",
                                 nosValveSlider,
                                 CommandType::NosValve,
                                 "To control the NOS valve -> [UNSAFE, ARM VALVE] need to be ON, [DUMP, FILL, ARM IGNITER] need to be OFF.",
                                 nosAndIpaValveSliderEnabled);

        renderPercentageInputRow("IPA Valve",
                                 ipaValveSlider,
                                 CommandType::IpaValve,
                                 "To control the IPA valve -> [UNSAFE, ARM VALVE] need to be ON, [DUMP, FILL, ARM IGNITER] need to be OFF.",
                                 nosAndIpaValveSliderEnabled);

        renderPercentageInputRow("Fill Valve",
                                 fillValveSlider,
                                 CommandType::FillValve,
                                 "To control the FILL valve -> [UNSAFE, FILL] need to be ON.",
                                 GSDataCenter::AllowFillSwitchData.isOn);

        renderPercentageInputRow("Dump Valve",
                                 dumpValveSlider,
                                 CommandType::DumpValve,
                                 "To control the DUMP valve -> [UNSAFE, DUMP] need to be ON.",
                                 GSDataCenter::AllowDumpSwitchData.isOn);

        // TODO: Only fully open or closed
        renderPercentageInputRow("Solenoid Valve",
                                 dumpHeatPadSlider,
                                 CommandType::DumpHeatPad,
                                 "To control the solenoid valve -> [UNSAFE] needs to be ON.",
                                 solenoidValveSliderEnabled);
        ImGui::EndTable();
    }

    ImGui::SeparatorText("Heat Pad Control");

    if (ImGui::BeginTable("fill_heat_pad_control_table", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Input");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Open Percentage");
        ImGui::TableSetupColumn("Set Value Button");

        renderPercentageInputRow("Nos Heat Pad", nosHeatPadSlider, CommandType::NosHeatPad);
        renderPercentageInputRow("Ipa Heat Pad", ipaHeatPadSlider, CommandType::IpaHeatPad);
        renderPercentageInputRow("Fill Heat Pad", fillHeatPadSlider, CommandType::FillHeatPad);

        ImGui::EndTable();
    }

    ImGui::SeparatorText("Switch to \"Prelaunch\"");
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(PrelaunchWindow::name);
    }
}

void FillWindow::renderPercentageInputRow(const char* name,
                                          PercentageInput& input,
                                          CommandType commandType,
                                          const char* tooltipDisabled,
                                          bool inputEnabled) const {
    ImGui::BeginDisabled(!inputEnabled);

    // Render int input
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::InputInt(name, &input.openedValue_perc);
    addDisabledTooltip(tooltipDisabled, inputEnabled);

    // Prevent out of bound percentage value
    if (input.openedValue_perc < 0) {
        input.openedValue_perc = 0;
    } else if (input.openedValue_perc > 100) {
        input.openedValue_perc = 100;
    }

    // Current value text
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("Open: %d%%", input.lastSetOpenedValue_perc);
    addDisabledTooltip(tooltipDisabled, inputEnabled);

    // Button to set the percentage value
    ImGui::TableSetColumnIndex(2);
    std::string buttonStr = std::string("Set Value") + "##" + name;
    if (ImGui::Button(buttonStr.c_str())) {
        input.lastSetOpenedValue_perc = input.openedValue_perc;
        CommandControl::sendCommand(commandType, input.openedValue_perc);
    }
    addDisabledTooltip(tooltipDisabled, inputEnabled);

    ImGui::EndDisabled();
}

void FillWindow::addDisabledTooltip(const char* tooltipDisabled, bool inputEnabled) const {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !inputEnabled) {
        ImGui::SetTooltip("%s", tooltipDisabled);
    }
}

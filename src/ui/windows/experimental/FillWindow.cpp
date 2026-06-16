#include "FillWindow.h"

#include "CommandControl.h"
#include "GSDataCenter.h"
#include "IniParams.h"
#include "PrelaunchWindow.h"
#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ThemedColors.h"
#include "units.h"

#include "system/state.hpp"

#include <imgui.h>
#include <implot.h>
#include <cstdint>

const char* const FillWindow::name = "Fill";

FillWindow::FillWindow()
    : tankLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(), PlotStyle("Tank Load Cell", ThemedColors::PlotLine::blue)}, 
      tankLoadCellADCPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(), PlotStyle("Tank Load Cell ADC", ThemedColors::PlotLine::blue)},
      tankTransducerPlotLine{GSDataCenter::PressureSensor_FillingStation_PlotData.p1().getValuePlotData(), PlotStyle("Tank Pressure", ThemedColors::PlotLine::red)},
      tankTransducerADCPlotLine{GSDataCenter::PressureSensor_FillingStation_PlotData.p1().getAdcPlotData(), PlotStyle("Tank Pressure ADC", ThemedColors::PlotLine::red)},
      tankTempPlotLine{GSDataCenter::Thermistor_FillingStation_PlotData.t1().getValuePlotData(), PlotStyle("Tank Temperature", ThemedColors::PlotLine::green)},
      tankTempADCPlotLine{GSDataCenter::Thermistor_FillingStation_PlotData.t1().getAdcPlotData(), PlotStyle("Tank Temperature ADC", ThemedColors::PlotLine::green)},
      tankMassPlotLine{GSDataCenter::TankMass_PlotData, PlotStyle("Tank Mass", ThemedColors::PlotLine::yellow)} {
}

const char* FillWindow::getName() const {
    return name;
}

void FillWindow::renderImpl() {
    constexpr Units::TimeUnit TIME_UNIT = Units::TimeUnit::Seconds;
    constexpr Units::WeightUnit WEIGHT_UNIT = Units::DEFAULT_WEIGHT_UNIT;
    constexpr Units::PressureUnit PRESSURE_UNIT = Units::DEFAULT_PRESSURE_UNIT;
    constexpr Units::TemperatureUnit TEMPERATURE_UNIT = Units::DEFAULT_TEMPERATURE_UNIT;
    constexpr Units::Unit ADC_UNIT = Units::QuantityUnit::Scalar;
    

    const bool nosAndIpaValveSliderEnabled = GSDataCenter::motorBoardState == static_cast<uint8_t>(logic::control::State::Unsafe) && GSDataCenter::ArmServoSwitchData.isOn
                                             && !GSDataCenter::AllowDumpSwitchData.isOn && !GSDataCenter::AllowFillSwitchData.isOn
                                             && !GSDataCenter::ArmIgniterSwitchData.isOn;
    const bool fillValveSliderEnabled = GSDataCenter::AllowFillSwitchData.isOn;
    const bool dumpValveSliderEnabled = GSDataCenter::AllowDumpSwitchData.isOn;
    const bool solenoidValveSliderEnabled = GSDataCenter::motorBoardState == static_cast<uint8_t>(logic::control::State::Unsafe);

    ImGui::SeparatorText("Tank Data Plot");

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Data", {-1.0f, 400.0f}, ImPlotFlags_NoInputs)) {
        constexpr ImAxis weightAxis = ImAxis_Y1;
        constexpr ImAxis pressureAxis = ImAxis_Y2;
        constexpr ImAxis tempAxis = ImAxis_Y3;

        ImPlot::SetupAxis(ImAxis_X1, Units::as_label(TIME_UNIT));
        ImPlot::SetupAxis(weightAxis, Units::as_label(WEIGHT_UNIT));
        ImPlot::SetupAxis(pressureAxis, Units::as_label(PRESSURE_UNIT));
        ImPlot::SetupAxis(tempAxis, Units::as_label(TEMPERATURE_UNIT));

        ImPlot::SetAxis(weightAxis);
        tankLoadCellPlotLine.plot(TIME_UNIT, WEIGHT_UNIT, IniParams::compressPlots.currentValue);
        tankMassPlotLine.plot(TIME_UNIT, WEIGHT_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::SetAxis(pressureAxis);
        tankTransducerPlotLine.plot(TIME_UNIT, PRESSURE_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::SetAxis(tempAxis);
        tankTempPlotLine.plot(TIME_UNIT, TEMPERATURE_UNIT, IniParams::compressPlots.currentValue);

        ImPlot::EndPlot();
    }

    if (ImPlot::BeginPlot("Tank Data ADC", {-1.0f, 400.0f}, ImPlotFlags_NoInputs)) {
        constexpr ImAxis weightAxis = ImAxis_Y1;
        constexpr ImAxis adcAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, Units::as_label(TIME_UNIT));
        ImPlot::SetupAxis(adcAxis, Units::as_label(ADC_UNIT));

        ImPlot::SetAxis(adcAxis);
        tankLoadCellADCPlotLine.plot(TIME_UNIT, ADC_UNIT, IniParams::compressPlots.currentValue, true);
        tankTransducerADCPlotLine.plot(TIME_UNIT, ADC_UNIT, IniParams::compressPlots.currentValue, true);
        tankTempADCPlotLine.plot(TIME_UNIT, ADC_UNIT, IniParams::compressPlots.currentValue, true);

        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Valve Control");

    if (ImGui::BeginTable("fill_valve_control_table", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Input");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Open Percentage");
        ImGui::TableSetupColumn("Set Value Button");

        renderPercentageInputRow("Fill Valve",
                                 fillValveSlider,
                                 CommandType::FillValve,
                                 {10, 25, 50, 75},
                                 "To control the FILL valve -> [UNSAFE, FILL] need to be ON.",
                                 GSDataCenter::AllowFillSwitchData.isOn);

        renderPercentageInputRow("Dump Valve",
                                 dumpValveSlider,
                                 CommandType::DumpValve,
                                 {10, 25, 50, 75},
                                 "To control the DUMP valve -> [UNSAFE, DUMP] need to be ON.",
                                 GSDataCenter::AllowDumpSwitchData.isOn);

        // TODO: Only fully open or closed
        renderPercentageInputRow("Solenoid Valve",
                                 dumpHeatPadSlider,
                                 CommandType::DumpHeatPad,
                                 {},
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
                                          std::initializer_list<uint32_t> presets,
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
    
    for (int i = 0; i < presets.size(); i++) {
        uint32_t preset = *(presets.begin() + i);

        if (i != 0)
            ImGui::SameLine();
        else
            ImGui::TableSetColumnIndex(3);
            
        std::string buttonStr = std::to_string(preset) + "%##" + name;
        if (ImGui::Button(buttonStr.c_str())) {
            input.openedValue_perc = preset;
            input.lastSetOpenedValue_perc = input.openedValue_perc;
            CommandControl::sendCommand(commandType, input.openedValue_perc);
        }
    } 

    ImGui::EndDisabled();
}

void FillWindow::addDisabledTooltip(const char* tooltipDisabled, bool inputEnabled) const {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && !inputEnabled) {
        ImGui::SetTooltip("%s", tooltipDisabled);
    }
}

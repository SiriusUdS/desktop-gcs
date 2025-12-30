#include "PrefillWindow.h"

#include "AppState.h"
#include "FillWindow.h"
#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"

#include <imgui.h>
#include <implot.h>

const char* const PrefillWindow::name = "Prefill";

PrefillWindow::PrefillWindow()
    : prewrapTankLoadCellState{AppState::TankLoadCell::prewrapADCValue, "Prewrap"},
      postwrapTankLoadCellState{AppState::TankLoadCell::postwrapADCValue, "Postwrap"},
      postIPATankLoadCellState{AppState::TankLoadCell::postIPAADCValue, "Post IPA"},
      tankLoadCellADCPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(),
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue)},
      tankLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell Weight", ThemedColors::PlotLine::red)} {
}

const char* PrefillWindow::getName() const {
    return name;
}

const char* PrefillWindow::getDockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void PrefillWindow::renderImpl() {
    ImGui::SeparatorText("Tests");

    ImGui::Text("Test: ");
    ImGui::SameLine();

    switch (sensorTestSequencer.currentTestType()) {
    case SensorTestSequencer::TestType::NOS_VALVE:
        ImGui::Text("NOS Valve");
        break;
    case SensorTestSequencer::TestType::IPA_VALVE:
        ImGui::Text("IPA Valve");
        break;
    case SensorTestSequencer::TestType::FILL_VALVE:
        ImGui::Text("Fill Valve");
        break;
    case SensorTestSequencer::TestType::DUMP_VALVE:
        ImGui::Text("Dump Valve");
        break;
    case SensorTestSequencer::TestType::NOS_HEATPAD:
        ImGui::Text("NOS Heat Pad");
        break;
    case SensorTestSequencer::TestType::IPA_HEATPAD:
        ImGui::Text("IPA Heat Pad");
        break;
    case SensorTestSequencer::TestType::FILL_HEATPAD:
        ImGui::Text("Fill Heat Pad");
        break;
    case SensorTestSequencer::TestType::DUMP_HEATPAD:
        ImGui::Text("Dump Heat Pad");
        break;
    default:
        ImGui::Text("None");
        break;
    }

    ImGui::Text("Test Action: ");
    ImGui::SameLine();

    switch (sensorTestSequencer.currentTestAction()) {
    case SensorTestSequencer::TestAction::OPEN_VALVE:
        ImGui::Text("Open Valve");
        break;
    case SensorTestSequencer::TestAction::CLOSE_VALVE:
        ImGui::Text("Close Valve");
        break;
    case SensorTestSequencer::TestAction::START_HEATPAD:
        ImGui::Text("Start Heat Pad");
        break;
    case SensorTestSequencer::TestAction::STOP_HEATPAD:
        ImGui::Text("Stop Heat Pad");
        break;
    default:
        ImGui::Text("None");
        break;
    }

    ImGui::BeginDisabled(sensorTestSequencer.isBusy());

    if (ImGui::BeginTable("PrefillTestTable", 2)) {
        ImGui::TableSetupColumn("Valve");
        ImGui::TableSetupColumn("Heat Pad");

        const ImVec2 buttonSize = {-1.0f, 0.0f};

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Test NOS Valve", buttonSize)) {
            sensorTestSequencer.testNOSValve();
        }

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Test NOS Heat Pad", buttonSize)) {
            sensorTestSequencer.testNOSHeatPad();
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Test IPA Valve", buttonSize)) {
            sensorTestSequencer.testIPAValve();
        }

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Test IPA Heat Pad", buttonSize)) {
            sensorTestSequencer.testIPAHeatPad();
        }

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Test Fill Valve", buttonSize)) {
            sensorTestSequencer.testFillValve();
        }

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Test Fill Heat Pad", buttonSize)) {
            sensorTestSequencer.testFillHeatPad();
        }

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Test Dump Valve", buttonSize)) {
            sensorTestSequencer.testDumpValve();
        }

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Test Dump Heat Pad", buttonSize)) {
            sensorTestSequencer.testDumpHeatPad();
        }

        ImGui::EndTable();
    }

    ImGui::EndDisabled();

    ImGui::SeparatorText("Calibration");

    ImGui::Text("Tank Load Cell ADC Values");

    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 6, ImGuiTableFlags_SizingFixedFit)) {
        const SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.tank();
        const float avgAdcValue = tankLoadCellData.getAdcPlotData().recentAverageValue();

        prewrapTankLoadCellState.renderAsRow(avgAdcValue);
        postwrapTankLoadCellState.renderAsRow(avgAdcValue);
        postIPATankLoadCellState.renderAsRow(avgAdcValue);

        ImGui::EndTable();
    }

    const ImVec2 plotSize = {-1.0f, 500.0f};
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Load Cell (ADC)", plotSize, ImPlotFlags_NoInputs)) {
        constexpr ImAxis adcValueAxis = ImAxis_Y1;
        constexpr ImAxis weightAxis = ImAxis_Y2;

        ImPlot::SetupAxis(ImAxis_X1, "Timestamp (ms)");
        ImPlot::SetupAxis(adcValueAxis, "ADC Value");
        ImPlot::SetupAxis(weightAxis, "Weight (lb)");

        ImPlot::SetAxis(adcValueAxis);
        tankLoadCellADCPlotLine.plot();

        ImPlot::SetAxis(weightAxis);
        tankLoadCellPlotLine.plot();

        ImPlot::EndPlot();
    }

    ImGui::SeparatorText("Switch to \"Fill\"");
    const bool allowConfirm = prewrapTankLoadCellState.saved && postwrapTankLoadCellState.saved && postIPATankLoadCellState.saved;
    ImGui::BeginDisabled(!allowConfirm);
    if (ImGui::Button("Confirm")) {
        ImGui::SetWindowFocus(FillWindow::name);
    }
    ImGui::EndDisabled();
    if (!allowConfirm && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("All calibration values need to be saved before proceeding to the \"Fill\" window.");
    }

    // TODO: THIS IS A TEST
    SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.tank();
    SensorPlotData& motorLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.motor();
    SensorPlotData& d1 = GSDataCenter::Thermistor_Motor_PlotData.tank();
    SensorPlotData& d2 = GSDataCenter::PressureSensor_Motor_PlotData.tank();
    static float t = 0.0f;
    tankLoadCellData.addData(t, t, t);
    motorLoadCellData.addData(t, t, t);
    d1.addData(t, t, t);
    d2.addData(t, t, t);
    t += 100.0f;
}

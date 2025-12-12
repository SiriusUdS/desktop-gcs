#include "PrefillWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"

#include <imgui.h>
#include <implot.h>

const char* PrefillWindow::name() const {
    return "Prefill";
}

const char* PrefillWindow::dockspace() const {
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

    /*const */ SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData[0];

    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Save");
        ImGui::TableSetupColumn("Cancel");
        ImGui::TableSetupColumn("ADC Value");

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Tank Load Cell ADC Value - Prewrap");

        ImGui::TableSetColumnIndex(1);
        ImGui::BeginDisabled(tankLoadCellADCValueSaved);
        if (ImGui::Button("Save##SaveTankLoadCellValue") && tankLoadCellData.getSize()) {
            tankLoadCellADCValue = tankLoadCellData.getAdcPlotData().latestValue();
            tankLoadCellADCValueSaved = true;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(2);
        ImGui::BeginDisabled(!tankLoadCellADCValueSaved);
        if (ImGui::Button("Cancel##CancelTankLoadCellValue")) {
            tankLoadCellADCValueSaved = false;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("ADC: %.0f", tankLoadCellADCValue);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Tank Load Cell ADC Value - Postwrap");

        ImGui::TableSetColumnIndex(1);
        ImGui::BeginDisabled(tankLoadCellADCValuePostWrapSaved);
        if (ImGui::Button("Save##SaveTankLoadCellValuePostWrap") && tankLoadCellData.getSize()) {
            tankLoadCellADCValuePostWrap = tankLoadCellData.getAdcPlotData().latestValue();
            tankLoadCellADCValuePostWrapSaved = true;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(2);
        ImGui::BeginDisabled(!tankLoadCellADCValuePostWrapSaved);
        if (ImGui::Button("Cancel##CancelTankLoadCellValuePostWrap")) {
            tankLoadCellADCValuePostWrapSaved = false;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("ADC: %.0f", tankLoadCellADCValuePostWrap);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Tank Load Cell ADC Value - Post IPA");

        ImGui::TableSetColumnIndex(1);
        ImGui::BeginDisabled(tankLoadCellADCValuePostIPASaved);
        if (ImGui::Button("Save##SaveTankLoadCellValuePostIPA") && tankLoadCellData.getSize()) {
            tankLoadCellADCValuePostIPA = tankLoadCellData.getAdcPlotData().latestValue();
            tankLoadCellADCValuePostIPASaved = true;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(2);
        ImGui::BeginDisabled(!tankLoadCellADCValuePostIPASaved);
        if (ImGui::Button("Cancel##CancelTankLoadCellValuePostIPA")) {
            tankLoadCellADCValuePostIPASaved = false;
        }
        ImGui::EndDisabled();

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("ADC: %.0f", tankLoadCellADCValuePostIPA);

        ImGui::EndTable();
    }

    ImGui::SeparatorText("Tank Load Cell Plot");

    const ImVec2 plotSize = {-1.0f, 500.0f};

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Load Cell (ADC)", plotSize, ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxes("Timestamp (ms)", "ADC Value");
        tankLoadCellData.plotAdc(true);
        ImPlot::EndPlot();
    }

    static float t = 0.0f;
    tankLoadCellData.addData(t, 0, t);
    t += 100.0f;
}

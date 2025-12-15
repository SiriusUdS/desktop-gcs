#include "PrefillWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "Params.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"

#include <imgui.h>
#include <implot.h>

PrefillWindow::PrefillWindow()
    : prewrapTankLoadCellParam{Params::TankLoadCell::prewrapADCValue, "Tank Load Cell ADC Value - Prewrap"},
      postwrapTankLoadCellParam{Params::TankLoadCell::postwrapADCValue, "Tank Load Cell ADC Value - Postwrap"},
      postIPATankLoadCellParam{Params::TankLoadCell::postIPAADCValue, "Tank Load Cell ADC Value - Post IPA"},
      tankLoadCellADCPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData[0].getAdcPlotData(), // TODO: is this correct idx?
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue)} {
}

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

    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 4, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Save");
        ImGui::TableSetupColumn("Cancel");
        ImGui::TableSetupColumn("ADC Value");

        renderTankLoadCellParam(prewrapTankLoadCellParam);
        renderTankLoadCellParam(postwrapTankLoadCellParam);
        renderTankLoadCellParam(postIPATankLoadCellParam);

        ImGui::EndTable();
    }

    ImGui::SeparatorText("Tank Load Cell Plot");

    const ImVec2 plotSize = {-1.0f, 500.0f};

    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("Tank Load Cell (ADC)", plotSize, ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxes("Timestamp (ms)", "ADC Value");
        tankLoadCellADCPlotLine.plot();
        ImPlot::EndPlot();
    }

    // TODO: THIS IS A TEST
    SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData[0];
    static float t = 0.0f;
    tankLoadCellData.addData(t, 0, t);
    t += 100.0f;
}

void PrefillWindow::renderTankLoadCellParam(TankLoadCellParam& tankLoadCellParam) {
    const SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData[0]; // TODO: is this correct idx?

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text(tankLoadCellParam.label.c_str());

    ImGui::TableSetColumnIndex(1);
    ImGui::BeginDisabled(tankLoadCellParam.saved);
    if (ImGui::Button(tankLoadCellParam.saveButtonLabel.c_str()) && tankLoadCellData.getAdcPlotData().getSize()) {
        tankLoadCellParam.param.currentValue = tankLoadCellData.getAdcPlotData().getValues().raw().back();
        tankLoadCellParam.saved = true;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(2);
    ImGui::BeginDisabled(!tankLoadCellParam.saved);
    if (ImGui::Button(tankLoadCellParam.cancelButtonLabel.c_str())) {
        tankLoadCellParam.saved = false;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(3);
    ImGui::Text("ADC: %.0f", tankLoadCellParam.param.currentValue);
}

PrefillWindow::TankLoadCellParam::TankLoadCellParam(FloatParam& param, std::string label) : param(param), label(label) {
    saveButtonLabel = "Save##save_" + param.iniKey;
    cancelButtonLabel = "Cancel##cancel_" + param.iniKey;
}

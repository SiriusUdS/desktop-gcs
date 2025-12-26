#include "PrefillWindow.h"

#include "GSDataCenter.h"
#include "ImGuiConfig.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "UiState.h"

#include <imgui.h>
#include <implot.h>

PrefillWindow::PrefillWindow()
    : prewrapTankLoadCellParam{UiState::TankLoadCell::prewrapADCValue, "Prewrap"},
      postwrapTankLoadCellParam{UiState::TankLoadCell::postwrapADCValue, "Postwrap"},
      postIPATankLoadCellParam{UiState::TankLoadCell::postIPAADCValue, "Post IPA"},
      tankLoadCellADCPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getAdcPlotData(),
                              PlotStyle("Tank Load Cell ADC Value", ThemedColors::PlotLine::blue)},
      tankLoadCellPlotLine{GSDataCenter::LoadCell_FillingStation_PlotData.motor().getValuePlotData(),
                           PlotStyle("Tank Load Cell Weight", ThemedColors::PlotLine::red)} {
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

    ImGui::Text("Tank Load Cell ADC Values");

    if (ImGui::BeginTable("PrefillTankLoadCellADCTable", 6, ImGuiTableFlags_SizingFixedFit)) {
        renderTankLoadCellParam(prewrapTankLoadCellParam);
        renderTankLoadCellParam(postwrapTankLoadCellParam);
        renderTankLoadCellParam(postIPATankLoadCellParam);

        ImGui::EndTable();
    }

    ImGui::SeparatorText("Tank Load Cell Plot");

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

void PrefillWindow::renderTankLoadCellParam(TankLoadCellParam& tankLoadCellParam) {
    const SensorPlotData& tankLoadCellData = GSDataCenter::LoadCell_FillingStation_PlotData.tank();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", tankLoadCellParam.label.c_str());

    ImGui::TableSetColumnIndex(1);
    ImGui::BeginDisabled(tankLoadCellParam.saved);
    if (ImGui::Button(tankLoadCellParam.readButtonLabel.c_str())) {
        tankLoadCellParam.readValue = tankLoadCellData.getValuePlotData().recentAverageValue();
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(2);
    ImGui::BeginDisabled(tankLoadCellParam.saved);
    if (ImGui::Button(tankLoadCellParam.saveButtonLabel.c_str())) {
        tankLoadCellParam.state.value = tankLoadCellParam.readValue;
        tankLoadCellParam.saved = true;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(3);
    ImGui::BeginDisabled(!tankLoadCellParam.saved);
    if (ImGui::Button(tankLoadCellParam.cancelButtonLabel.c_str())) {
        tankLoadCellParam.saved = false;
        tankLoadCellParam.readValue = 0;
    }
    ImGui::EndDisabled();

    ImGui::TableSetColumnIndex(4);
    ImGui::Text("ADC Value: %.0f", tankLoadCellParam.readValue);

    ImGui::TableSetColumnIndex(5);
    if (tankLoadCellParam.saved) {
        ImVec4 color = ThemedColors::Text::green.resolve();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("SAVED");
    } else {
        ImVec4 color = ThemedColors::Text::red.resolve();
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("UNSAVED");
    }
    ImGui::PopStyleColor();
}

PrefillWindow::TankLoadCellParam::TankLoadCellParam(SessionState& state, std::string label) : state(state), label(label) {
    readButtonLabel = "Read##read_" + state.id;
    saveButtonLabel = "Save##save_" + state.id;
    cancelButtonLabel = "Cancel##cancel_" + state.id;
}

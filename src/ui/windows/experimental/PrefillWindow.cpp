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

    if (ImGui::Button("Test NOS Valve")) {
        sensorTestSequencer.testNOSValve();
    }

    if (ImGui::Button("Test IPA Valve")) {
        sensorTestSequencer.testIPAValve();
    }

    if (ImGui::Button("Test Fill Valve")) {
        sensorTestSequencer.testFillValve();
    }

    if (ImGui::Button("Test Dump Valve")) {
        sensorTestSequencer.testDumpValve();
    }

    if (ImGui::Button("Test NOS Heat Pad")) {
        sensorTestSequencer.testNOSHeatPad();
    }

    if (ImGui::Button("Test IPA Heat Pad")) {
        sensorTestSequencer.testIPAHeatPad();
    }

    if (ImGui::Button("Test Fill Heat Pad")) {
        sensorTestSequencer.testFillHeatPad();
    }

    if (ImGui::Button("Test Dump Heat Pad")) {
        sensorTestSequencer.testDumpHeatPad();
    }

    ImGui::EndDisabled();

    ImGui::SeparatorText("Calibration");

    constexpr float calibrationButtonsOffsetX = 600.0f;

    if (ImGui::Button("Save ADC Tank Load Cell Value")) {
    }
    ImGui::SameLine(calibrationButtonsOffsetX);
    ImGui::Text("ADC: %.0f", 10.2f);

    if (ImGui::Button("Save ADC Tank Load Cell Value - Post Wrap")) {
    }
    ImGui::SameLine(calibrationButtonsOffsetX);
    ImGui::Text("ADC: %.0f", 15.9f);

    if (ImGui::Button("Save ADC Tank Load Cell Value - Post IPA")) {
    }
    ImGui::SameLine(calibrationButtonsOffsetX);
    ImGui::Text("ADC: %.0f", 22.3f);

    ImGui::SeparatorText("Tank Load Cell Plot");

    ImPlot::BeginPlot("Tank Load Cell", {-1, -1});
    GSDataCenter::LoadCell_FillingStation_PlotData[0].plotAdc(true);
    ImPlot::EndPlot();
}

#include "NOSPhaseDiagramWindow.h"

#include "DataSeries.h"
#include "GSDataCenter.h"
#include "VaporPressure.h"

#include <implot.h>

const NOSPhaseDiagramWindow::PT_Point NOSPhaseDiagramWindow::triplePoint{-90.82f, 12.74156526f};
const NOSPhaseDiagramWindow::PT_Point NOSPhaseDiagramWindow::criticalPoint{36.42f, 1050.7984099f};

const float NOSPhaseDiagramWindow::minTempAntoineEquation_C = -10.f;
const float NOSPhaseDiagramWindow::maxTempAntoineEquation_C = 40.f;

void NOSPhaseDiagramWindow::init() {
    vaporizationCurveTemperatures.clear();
    vaporizationCurvePressures.clear();
    topLine.clear();
    bottomLine.clear();

    for (float temp_C = minTempAntoineEquation_C; temp_C <= maxTempAntoineEquation_C; temp_C += .1f) {
        const float pressure_psi = static_cast<float>(VaporPressure::vaporPressureNOS_psi(temp_C));
        vaporizationCurveTemperatures.push_back(temp_C);
        vaporizationCurvePressures.push_back(pressure_psi);
    }

    topLine.resize(static_cast<int>(vaporizationCurveTemperatures.size()));
    for (int i = 0; i < topLine.size(); ++i) {
        topLine[i] = criticalPoint.y;
    }

    bottomLine.resize(static_cast<int>(vaporizationCurveTemperatures.size()));
    for (int i = 0; i < bottomLine.size(); ++i) {
        bottomLine[i] = 0.0f;
    }
}

const char* NOSPhaseDiagramWindow::getName() const {
    return "NOS Phase Diagram";
}

void NOSPhaseDiagramWindow::renderImpl() {
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("NOS Phase Diagram", ImGui::GetContentRegionAvail(), ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxes("Temperature (C)", "Pressure (psi)");

        ImPlot::PushStyleColor(ImPlotCol_Fill, IM_COL32(40, 120, 255, 80)); // Blue
        ImPlot::PlotShaded("Liquid phase",
                           vaporizationCurveTemperatures.data(),
                           vaporizationCurvePressures.data(),
                           topLine.begin(),
                           static_cast<int>(vaporizationCurveTemperatures.size()));
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Fill, IM_COL32(255, 140, 40, 80)); // Orange
        ImPlot::PlotShaded("Gas phase",
                           vaporizationCurveTemperatures.data(),
                           vaporizationCurvePressures.data(),
                           bottomLine.begin(),
                           static_cast<int>(vaporizationCurveTemperatures.size()));
        ImPlot::PopStyleColor();

        ImPlot::SetNextLineStyle({1, 0, 0, 1}, 3); // Red
        ImPlot::PlotLine("Vaporization curve",
                         vaporizationCurveTemperatures.data(),
                         vaporizationCurvePressures.data(),
                         static_cast<int>(vaporizationCurveTemperatures.size()));

        const ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 12.0f, textColor, 2.0f, textColor); // Black
        float tankTemperature_C = GSDataCenter::Thermistor_Motor_PlotData.tank().getSize()
                                    ? GSDataCenter::Thermistor_Motor_PlotData.tank().getValuePlotData().getValues().raw().back()
                                    : 0;
        float tankPressure_psi = GSDataCenter::PressureSensor_Motor_PlotData.tank().getSize()
                                   ? GSDataCenter::PressureSensor_Motor_PlotData.tank().getValuePlotData().getValues().raw().back()
                                   : 0;
        ImPlot::PlotScatter("Current state", &tankTemperature_C, &tankPressure_psi, 1);

        ImPlot::EndPlot();
    }
}

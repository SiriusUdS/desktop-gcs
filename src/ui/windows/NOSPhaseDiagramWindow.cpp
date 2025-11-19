#include "NOSPhaseDiagramWindow.h"

#include "GSDataCenter.h"
#include "PlotRawData.h"
#include "VaporPressure.h"

#include <cmath>
#include <imgui.h>
#include <implot.h>

namespace NOSPhaseDiagramWindow {
using PT_Point = ImVec2; // Point type for Pressure-Temperature diagram (Temperature (C), Pressure (psi))

constexpr PT_Point triplePoint{-90.82f, 12.74156526f};
constexpr PT_Point criticalPoint{36.42f, 1050.7984099f};

constexpr float minTempAntoineEquation_C = -10.f;
constexpr float maxTempAntoineEquation_C = 40.f;

PlotRawData vaporizationCurve;
ImVector<float> topLine;
ImVector<float> bottomLine;
} // namespace NOSPhaseDiagramWindow

void NOSPhaseDiagramWindow::init() {
    vaporizationCurve.clear();
    topLine.clear();
    bottomLine.clear();

    for (float temp_C = minTempAntoineEquation_C; temp_C <= maxTempAntoineEquation_C; temp_C += .1f) {
        const float pressure_psi = static_cast<float>(VaporPressure::vaporPressureNOS_psi(temp_C));
        vaporizationCurve.add(temp_C, pressure_psi);
    }

    topLine.resize(static_cast<int>(vaporizationCurve.size()));
    for (int i = 0; i < topLine.size(); ++i) {
        topLine[i] = criticalPoint.y;
    }

    bottomLine.resize(static_cast<int>(vaporizationCurve.size()));
    for (int i = 0; i < bottomLine.size(); ++i) {
        bottomLine[i] = 0.0f;
    }
}

void NOSPhaseDiagramWindow::render() {
    ImPlot::SetNextAxesToFit();
    if (ImPlot::BeginPlot("NOS Phase Diagram", ImGui::GetContentRegionAvail(), ImPlotFlags_NoInputs)) {
        ImPlot::SetupAxes("Temperature (C)", "Pressure (psi)");

        ImPlot::PushStyleColor(ImPlotCol_Fill, IM_COL32(40, 120, 255, 80)); // Blue
        ImPlot::PlotShaded("Liquid phase",
                           vaporizationCurve.getRawX(),
                           vaporizationCurve.getRawY(),
                           topLine.begin(),
                           static_cast<int>(vaporizationCurve.size()));
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Fill, IM_COL32(255, 140, 40, 80)); // Orange
        ImPlot::PlotShaded("Gas phase",
                           vaporizationCurve.getRawX(),
                           vaporizationCurve.getRawY(),
                           bottomLine.begin(),
                           static_cast<int>(vaporizationCurve.size()));
        ImPlot::PopStyleColor();

        ImPlot::SetNextLineStyle({1, 0, 0, 1}, 3); // Red
        ImPlot::PlotLine("Vaporization curve", vaporizationCurve.getRawX(), vaporizationCurve.getRawY(), static_cast<int>(vaporizationCurve.size()));

        const ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 12.0f, textColor, 2.0f, textColor); // Black
        ImPlot::PlotScatter("Current state", &GSDataCenter::tankTemperature_C, &GSDataCenter::tankPressure_psi, 1);

        ImPlot::EndPlot();
    }
}

#include "PlotWindow.h"

#include "FontConfig.h"
#include "IniConfig.h"
#include "StringUtils.h"
#include "ThemedColors.h"

#include <imgui.h>
#include <implot.h>
#include <iomanip>
#include <sstream>

PlotWindow::PlotWindow(const char* name, const char* xLabel, Units::Unit xUnit, const char* yLabel, Units::Unit yUnit, Units::Unit adcUnit, std::vector<SensorPlotParam> sensorPlotParamsVec): name(name), xLabel(xLabel), xUnit(xUnit), yLabel(yLabel), yUnit(yUnit), adcUnit(adcUnit) {
    for (auto& params : sensorPlotParamsVec) {
        sensorPlotLineVec.push_back({params.data, params.style});
    }

    autofitIniId = std::string(name) + "_autofit";
    showAvgValuesId = std::string(name) + "_avg";
    plotModeIniId = std::string(name) + "_plot_mode";
    timeWindowIniId = std::string(name) + "_twindow";

    StringUtils::convertStringToIniId(autofitIniId);
    StringUtils::convertStringToIniId(showAvgValuesId);
    StringUtils::convertStringToIniId(plotModeIniId);
    StringUtils::convertStringToIniId(timeWindowIniId);
}

float PlotWindow::timeWindowToMs(TimeWindowMode mode)
{
    switch (mode) {
        case TimeWindowMode::Last2s:  return 2000.f;
        case TimeWindowMode::Last10s: return 10000.f;
        case TimeWindowMode::Last30s: return 30000.f;
        default: return 0.f;
    }
}

std::string PlotWindow::formatLabel(std::string label, Units::Unit unit) {
    std::string unitSymbol = Units::as_symbol(unit);
    std::string unitLabel  = Units::as_label(unit);

    if (label.empty()) {
        label = unitLabel;
    }

    std::string labelText = label;

    if (!unitSymbol.empty()) {
        if (!labelText.empty()) {
            labelText += " ";
        }
        labelText += "(" + unitSymbol + ")";
    }

    return labelText;
}

void PlotWindow::render()
{
    ImGui::Checkbox("Auto-fit", &autofit);
    ImGui::SameLine();
    ImGui::Checkbox("Avg", &showAvgValues);
    ImGui::SameLine();

    ImGui::RadioButton("Value", reinterpret_cast<int*>(&plotMode), 0);
    ImGui::SameLine();
    ImGui::RadioButton("ADC", reinterpret_cast<int*>(&plotMode), 1);

    ImGui::SameLine();
    ImGui::Combo("Time", reinterpret_cast<int*>(&timeWindow), "All\0Last 2s\0Last 10s\0Last 30s\0");

    if (ImGui::Button("Clear data")) {
        for (auto& s : sensorPlotLineVec) {
            s.data.clear();
        }
    }

    if (autofit) {
        flags = ImPlotFlags_NoInputs;
        ImPlot::SetNextAxesToFit();
    } else {
        flags = ImPlotFlags_None;
    }

    if (!ImPlot::BeginPlot(name.c_str(), ImGui::GetContentRegionAvail(), flags))
        return;

    ImPlot::SetupAxes(formatLabel(xLabel, xUnit).c_str(), formatLabel((plotMode == PlotMode::ADC) ? "ADC" : yLabel, yUnit).c_str());

    const float windowMs = timeWindowToMs(static_cast<TimeWindowMode>(timeWindow));

    const size_t avgWindowMs = 2000;

    for (size_t i = 0; i < sensorPlotLineVec.size(); i++) {
        auto& sensor = sensorPlotLineVec[i];

        const PlotData& plotData = sensor.data.getPlotData(plotMode);

        PlotLine line(plotData, sensor.style);
        line.plot(plotData.getTimeline().getUnit(), yUnit);

        if (showAvgValues) {
            float avg = plotData.recentAverageValue(avgWindowMs);
            showAvgRecentValue(sensor.style.name, avg, i);
        }
    }

    if (showAvgValues) {
        showAvgRecentLabel(avgWindowMs / 1000);
    }

    ImPlot::EndPlot();
}

void PlotWindow::loadState(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        auto sec = ini.get(IniConfig::GCS_SECTION);

        if (sec.has(autofitIniId))
            autofit = std::stoi(sec.get(autofitIniId));

        if (sec.has(showAvgValuesId))
            showAvgValues = std::stoi(sec.get(showAvgValuesId));

        if (sec.has(plotModeIniId))
            plotMode = static_cast<PlotMode>(std::stoi(sec.get(plotModeIniId)));

        if (sec.has(timeWindowIniId))
            timeWindow = static_cast<TimeWindowMode>(std::stoi(sec.get(timeWindowIniId)));
    }
}

void PlotWindow::saveState(mINI::INIStructure& ini) {
    ini[IniConfig::GCS_SECTION].set(autofitIniId, std::to_string(autofit));
    ini[IniConfig::GCS_SECTION].set(showAvgValuesId, std::to_string(showAvgValues));
    ini[IniConfig::GCS_SECTION].set(plotModeIniId, std::to_string(static_cast<int>(plotMode)));
    ini[IniConfig::GCS_SECTION].set(timeWindowIniId, std::to_string(static_cast<int>(timeWindow)));
}

std::string PlotWindow::getWindowId() {
    return "Plot - " + name;
}

void PlotWindow::showAvgRecentLabel(const size_t durationSec) {
    std::string txt = "Avg last " + std::to_string(durationSec) + "s";

    ImDrawList* dl = ImPlot::GetPlotDrawList();

    ImVec2 pos = ImPlot::GetPlotPos();
    ImVec2 size = ImPlot::GetPlotSize();
    ImVec2 ts = ImGui::CalcTextSize(txt.c_str());

    ImVec2 p = {pos.x + size.x - ts.x - 10.f, pos.y + 10.f};

    dl->AddText(p, IM_COL32_WHITE, txt.c_str());
}

void PlotWindow::showAvgRecentValue(const char* name, float value, size_t idx) {
    std::ostringstream oss;
    oss << name << ": " << std::fixed << std::setprecision(1) << value;

    std::string txt = oss.str();

    float fontSize = 25.f;

    ImDrawList* dl = ImPlot::GetPlotDrawList();

    ImVec2 pos = ImPlot::GetPlotPos();
    ImVec2 size = ImPlot::GetPlotSize();

    ImVec2 ts = FontConfig::mainFont->CalcTextSizeA(fontSize, FLT_MAX, -1.f, txt.c_str());

    ImVec2 p = {pos.x + size.x - ts.x - 10.f, pos.y + (ts.y + 2.f) * (idx + 1) + 20.f};

    dl->AddText(FontConfig::mainFont, fontSize, p, IM_COL32_WHITE,txt.c_str());
}
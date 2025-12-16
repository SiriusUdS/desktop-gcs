#include "PlotWindow.h"

#include "FontConfig.h"
#include "IniConfig.h"
#include "StringUtils.h"

#include <imgui.h>
#include <implot.h>
#include <ini.h>
#include <iomanip>

/**
 * @brief Constructs a plot window.
 * @param name Name of the plot window.
 * @param xLabel Text to display along the X axis.
 * @param yLabel Text to display along the Y axis.
 * @param sensorPlotParamsVec A vector of the params of all the sensor plot lines to be displayed in this window.
 */
PlotWindow::PlotWindow(const char* name, const char* xLabel, const char* yLabel, std::vector<SensorPlotParam> sensorPlotParamsVec)
    : name(name), xLabel(xLabel), yLabel(yLabel) {
    for (auto& params : sensorPlotParamsVec) {
        sensorPlotLineVec.emplace_back(params.data,
                                       PlotLine(params.data.getValuePlotData(), params.style),
                                       PlotLine(params.data.getAdcPlotData(), params.style));
    }

    autofitIniId = std::string(name) + "_plot_window_auto_fit";
    showCompressedDataIniId = std::string(name) + "_plot_window_show_compressed_data";
    showAvgValuesId = std::string(name) + "_plot_window_show_avg_values";
    dataTypeIniId = std::string(name) + "_plot_window_data_type";
    StringUtils::convertStringToIniId(autofitIniId);
    StringUtils::convertStringToIniId(showCompressedDataIniId);
    StringUtils::convertStringToIniId(showAvgValuesId);
    StringUtils::convertStringToIniId(dataTypeIniId);
}

/**
 * @brief Renders the plot window with ImGui.
 */
void PlotWindow::render() {
    ImGui::Checkbox("Auto-fit", &autofit);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Automatically adjusts the view to fit all data. Disables zooming and panning.");
    }
    ImGui::SameLine();
    ImGui::Checkbox("Compressed", &showCompressedData);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Reduces the number of displayed data points to improve performance.");
    }
    ImGui::SameLine();
    ImGui::Checkbox("Avg. values", &showAvgValues);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Shows the recent average of each plot line.");
    }
    ImGui::SameLine();
    ImGui::RadioButton("Value", &dataType, VALUE);
    ImGui::SameLine();
    ImGui::RadioButton("ADC", &dataType, ADC);
    ImGui::SameLine();

    if (ImGui::Button("Clear data")) {
        for (auto& sensorPlotLine : sensorPlotLineVec) {
            sensorPlotLine.data.clear();
        }
    }

    if (autofit) {
        flags = ImPlotFlags_NoInputs;
        ImPlot::SetNextAxesToFit();
    } else {
        flags = ImPlotFlags_None;
    }

    if (ImPlot::BeginPlot(name.c_str(), ImGui::GetContentRegionAvail(), flags)) {
        const char* actualYLabel = dataType == ADC ? "ADC Value" : yLabel.c_str();
        ImPlot::SetupAxes(xLabel.c_str(), actualYLabel);

        static constexpr size_t recentAvgValueDurationMs = 2000;
        static constexpr size_t recentAvgValueDurationSec = recentAvgValueDurationMs / 1000;

        for (size_t i = 0; i < sensorPlotLineVec.size(); i++) {
            const SensorPlotLine& sensorPlotLine = sensorPlotLineVec[i];
            const PlotLine& plotLine = dataType == VALUE ? sensorPlotLine.valuePlotLine : sensorPlotLine.adcPlotLine;

            plotLine.plot(showCompressedData);
            if (showAvgValues) {
                showAvgRecentValue(plotLine.getStyle().name, plotLine.getData().recentAverageValue(recentAvgValueDurationMs), i);
            }
        }

        if (showAvgValues) {
            showAvgRecentLabel(recentAvgValueDurationSec);
        }

        ImPlot::EndPlot();
    }
}

/**
 * @brief Loads the state of the window from an ini file.
 * @param ini The struct of the ini file.
 */
void PlotWindow::loadState(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        if (ini.get(IniConfig::GCS_SECTION).has(autofitIniId)) {
            autofit = std::stoi(ini.get(IniConfig::GCS_SECTION).get(autofitIniId));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(showCompressedDataIniId)) {
            showCompressedData = std::stoi(ini.get(IniConfig::GCS_SECTION).get(showCompressedDataIniId));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(showAvgValuesId)) {
            showAvgValues = std::stoi(ini.get(IniConfig::GCS_SECTION).get(showAvgValuesId));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(dataTypeIniId)) {
            dataType = std::stoi(ini.get(IniConfig::GCS_SECTION).get(dataTypeIniId));
        }
    }
}

/**
 * @brief Saves the state of the window to an ini file.
 * @param ini The struct of the ini file.
 */
void PlotWindow::saveState(mINI::INIStructure& ini) {
    ini[IniConfig::GCS_SECTION].set(autofitIniId, std::to_string(autofit));
    ini[IniConfig::GCS_SECTION].set(showCompressedDataIniId, std::to_string(showCompressedData));
    ini[IniConfig::GCS_SECTION].set(showAvgValuesId, std::to_string(showAvgValues));
    ini[IniConfig::GCS_SECTION].set(dataTypeIniId, std::to_string(dataType));
}

/**
 * @brief Generates a window id to uniquely identify different plot windows.
 * @returns The generatsed window id.
 */
std::string PlotWindow::getWindowId() {
    return "Plot - " + name;
}

void PlotWindow::showAvgRecentLabel(const size_t durationSec) {
    const std::string avgValueLabelStr = "Avg. values in the last " + std::to_string(durationSec) + "s";
    const char* avgValueLabelText = avgValueLabelStr.c_str();

    ImDrawList* drawList = ImPlot::GetPlotDrawList();
    const ImVec2 plotPos = ImPlot::GetPlotPos();
    const ImVec2 plotSize = ImPlot::GetPlotSize();
    const ImVec2 textSize = ImGui::CalcTextSize(avgValueLabelText);
    const ImVec2 textPos = {plotPos.x + plotSize.x - textSize.x - 10.f, plotPos.y + 10.f};
    const ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
    drawList->AddText(textPos, textColor, avgValueLabelText);

    constexpr float underlineThickness = 1.0f;
    constexpr float underlineOffsetY = -3.0f;
    ImVec2 underlineStart = {textPos.x, textPos.y + textSize.y + underlineOffsetY};
    ImVec2 underlineEnd = {textPos.x + textSize.x, textPos.y + textSize.y + underlineOffsetY};
    drawList->AddLine(underlineStart, underlineEnd, textColor, underlineThickness);
}

void PlotWindow::showAvgRecentValue(const char* name, float value, size_t idx) {
    std::ostringstream oss;
    oss << name << ": " << std::fixed << std::setprecision(1) << value;
    const std::string avgValueStr = oss.str();
    const char* avgValueText = avgValueStr.c_str();

    constexpr float fontSize = 25.f;
    constexpr float spacingBetweenLines = 2.f;

    ImDrawList* drawList = ImPlot::GetPlotDrawList();
    const ImVec2 plotPos = ImPlot::GetPlotPos();
    const ImVec2 plotSize = ImPlot::GetPlotSize();
    const ImVec2 textSize = FontConfig::mainFont->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, avgValueText);
    const ImVec2 textPos = {plotPos.x + plotSize.x - textSize.x - 10.f, plotPos.y + (textSize.y + spacingBetweenLines) * (idx + 1) + 20.f};
    const ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
    drawList->AddText(FontConfig::mainFont, fontSize, textPos, textColor, avgValueText);
}

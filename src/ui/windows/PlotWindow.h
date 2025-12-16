#pragma once

#include "PlotLine.h"
#include "PlotStyle.h"
#include "SensorPlotData.h"

#include <implot.h>
#include <ini.h>
#include <vector>

/**
 * @class PlotWindow
 * @brief UI window to display plot data
 */
class PlotWindow {
public:
    struct SensorPlotParam {
        SensorPlotData& data;
        PlotStyle style;
    };

public:
    PlotWindow(const char* name, const char* xLabel, const char* yLabel, std::vector<SensorPlotParam> sensorPlotParamsVec);
    void render();
    void loadState(const mINI::INIStructure& ini);
    void saveState(mINI::INIStructure& ini);
    std::string getWindowId();

private:
    struct SensorPlotLine {
        SensorPlotData& data;
        PlotLine valuePlotLine;
        PlotLine adcPlotLine;
    };

    enum DataType { VALUE = 0, ADC = 1 };

    void showAvgRecentLabel(const size_t durationSec);
    void showAvgRecentValue(const char* name, float value, size_t idx);

    std::string name, xLabel, yLabel, autofitIniId, showCompressedDataIniId, showAvgValuesId, dataTypeIniId;
    std::vector<SensorPlotLine> sensorPlotLineVec;
    ImPlotFlags flags{};
    bool autofit{};
    bool showCompressedData{};
    bool showAvgValues{};
    int dataType{};
}; // namespace PlotWindow

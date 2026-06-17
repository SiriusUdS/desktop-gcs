#pragma once

#include "PlotLine.h"
#include "PlotStyle.h"
#include "SensorPlotData.h"
#include "units.h"

#include <implot.h>
#include <ini.h>
#include <vector>
#include <string>

/**
 * @class PlotWindow
 * @brief UI window to display plot data
 */
class PlotWindow {
public:
    enum class TimeWindowMode {
        All,
        Last2s,
        Last10s,
        Last30s
    };

    struct SensorPlotParam {
        SensorPlotData& data;
        PlotStyle style;
    };

private:
    struct SensorPlotSeries {
        SensorPlotData& data;
        PlotStyle style;
    };

public:
    PlotWindow(const char* name, const char* xLabel, Units::Unit xUnit, const char* yLabel, Units::Unit yUnit, Units::Unit adcUnit, std::vector<SensorPlotParam> sensorPlotParamsVec);

    void render();
    void loadState(const mINI::INIStructure& ini);
    void saveState(mINI::INIStructure& ini);
    std::string getWindowId();

private:
    void showAvgRecentLabel(const size_t durationSec);
    void showAvgRecentValue(const char* name, float value, size_t idx);

    static float timeWindowToMs(TimeWindowMode mode);

    std::string formatLabel(std::string label, Units::Unit unit);

private:
    std::string name;
    std::string xLabel;
    std::string yLabel;

    Units::Unit xUnit;
    Units::Unit yUnit;
    Units::Unit adcUnit;

    std::string autofitIniId;
    std::string showAvgValuesId;
    std::string plotModeIniId;
    std::string timeWindowIniId;

    std::vector<SensorPlotSeries> sensorPlotLineVec;

    ImPlotFlags flags;

    bool autofit;
    bool showAvgValues;
    PlotMode plotMode{PlotMode::Value};
    TimeWindowMode timeWindow{TimeWindowMode::All};
};
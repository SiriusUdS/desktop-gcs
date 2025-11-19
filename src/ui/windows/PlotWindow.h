#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <implot.h>
#include <ini.h>
#include <vector>

class SensorPlotData;

/**
 * @class PlotWindow
 * @brief UI window to display plot data
 */
class PlotWindow {
public:
    PlotWindow(const char* name, const char* xLabel, const char* yLabel, std::vector<SensorPlotData*> sensorPlotDataVec);
    void render();
    void loadState(const mINI::INIStructure& ini);
    void saveState(mINI::INIStructure& ini);
    std::string getWindowId();

private:
    enum DataType { VALUE = 0, ADC = 1 };

    void showAvgRecentLabel(const size_t durationSec);
    void showAvgRecentValue(const char* name, float value, size_t idx);

    std::string name, xLabel, yLabel, autofitIniId, showCompressedDataIniId, showAvgValuesId, dataTypeIniId;
    std::vector<SensorPlotData*> sensorPlotDataVec;
    ImPlotFlags flags{};
    bool autofit{};
    bool showCompressedData{};
    bool showAvgValues{};
    int dataType{};
}; // namespace PlotWindow

#endif // PLOTWINDOW_H

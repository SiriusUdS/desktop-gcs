#ifndef RECENTPLOTDATA_H
#define RECENTPLOTDATA_H

#include <imgui.h>

class PlotData;
struct ThemedColor;

class RecentPlotData {
public:
    RecentPlotData(const PlotData& plotData, size_t windowX);
    RecentPlotData(const PlotData& plotData, size_t windowX, const ThemedColor& colorOverride);
    void plot(bool showCompressedData);

private:
    const PlotData& plotData;
    size_t windowX;
    size_t start{};
    const ThemedColor& color;
};

#endif // RECENTPLOTDATA_H

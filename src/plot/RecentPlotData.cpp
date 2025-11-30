#include "RecentPlotData.h"

#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

RecentPlotData::RecentPlotData(const PlotData& plotData, size_t windowX) : plotData(plotData), windowX(windowX), color(plotData.getColor()) {
}

RecentPlotData::RecentPlotData(const PlotData& plotData, size_t windowX, const ThemedColor& colorOverride)
    : plotData(plotData), windowX(windowX), color(colorOverride) {
}

void RecentPlotData::plot(bool showCompressedData) {
    PlotData::LockedView view = plotData.makeLockedView();
    const PlotRawData& data = showCompressedData ? view.getCompressedData() : view.getData();
    const PlotStyle& style = view.getStyle();
    const size_t size = data.size();

    if (start >= size) {
        start = 0;
    }

    if (size > 0) {
        const float minX = data.lastX() - windowX;

        while (start > 0 && data.getXAt(start - 1) > minX) {
            start--;
        }

        while (start < size && data.getXAt(start) < minX) {
            start++;
        }
    }

    const size_t visibleCount = size - start;
    ImPlot::SetNextLineStyle(color.resolve(), style.weight);
    ImPlot::PlotLine(style.name, data.getRawX() + start, data.getRawY() + start, static_cast<int>(visibleCount));
}

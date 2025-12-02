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
    const std::vector<float>& timeline = showCompressedData ? view.getTimeline().compressed() : view.getTimeline().raw();
    const std::vector<float>& values = showCompressedData ? view.getValues().compressed() : view.getValues().raw();
    const PlotStyle& style = view.getStyle();
    const size_t size = timeline.size();

    if (start >= size) {
        start = 0;
    }

    if (size > 0) {
        const float minX = values.back() - windowX;

        while (start > 0 && values.at(start - 1) > minX) {
            start--;
        }

        while (start < size && values.at(start) < minX) {
            start++;
        }
    }

    const size_t visibleCount = size - start;
    ImPlot::SetNextLineStyle(color.resolve(), style.weight);
    ImPlot::PlotLine(style.name, timeline.data() + start, values.data() + start, static_cast<int>(visibleCount));
}

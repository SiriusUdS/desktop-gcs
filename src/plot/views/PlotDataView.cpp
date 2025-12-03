#include "PlotDataView.h"

#include "DataSelector.h"
#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

PlotDataView::PlotDataView(const PlotData& plotData) : plotData(plotData), color(plotData.getColor()) {
}

PlotDataView::PlotDataView(const PlotData& plotData, const ThemedColor& colorOverride) : plotData(plotData), color(colorOverride) {
}

void PlotDataView::plot(bool showCompressedData) {
    PlotData::LockedView view = plotData.makeLockedView();
    const std::vector<float>& timeline = showCompressedData ? view.getTimeline().compressed() : view.getTimeline().raw();
    const std::vector<float>& values = showCompressedData ? view.getValues().compressed() : view.getValues().raw();
    const PlotStyle& style = view.getStyle();
    const size_t size = timeline.size();

    DataSelector::Window window = getDataSelectorWindow(showCompressedData, timeline);

    ImPlot::SetNextLineStyle(color.resolve(), style.weight);
    ImPlot::PlotLine(style.name, timeline.data() + window.start, values.data() + window.start, static_cast<int>(window.size));
}

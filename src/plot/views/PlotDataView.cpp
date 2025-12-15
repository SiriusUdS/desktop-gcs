#include "PlotDataView.h"

#include "DataSelector.h"
#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

PlotDataView::PlotDataView(const PlotData& data, const PlotStyle& style) : data(data), style(style) {
}

void PlotDataView::plot(bool showCompressedData) {
    PlotData::LockedView view = data.makeLockedView();
    const std::vector<float>& timeline = showCompressedData ? view.getTimeline().compressed() : view.getTimeline().raw();
    const std::vector<float>& values = showCompressedData ? view.getValues().compressed() : view.getValues().raw();

    DataSelector::Window window = getDataSelectorWindow(showCompressedData, timeline);

    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    ImPlot::PlotLine(style.name, timeline.data() + window.start, values.data() + window.start, static_cast<int>(timeline.size()));
}

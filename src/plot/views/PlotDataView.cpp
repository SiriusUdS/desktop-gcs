#include "PlotDataView.h"

#include "DataSelector.h"
#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

PlotDataView::PlotDataView(const PlotData& data, const PlotStyle& style) : data(data), style(style) {
}

void PlotDataView::plot(Units::Unit time_unit, Units::Unit unit, bool showCompressedData, bool hidden) {
    PlotData::LockedView view = data.makeLockedView();
    const std::vector<float>& timeline = showCompressedData ? view.getTimeline().compressed(time_unit) : view.getTimeline().raw(time_unit);
    const std::vector<float>& values = showCompressedData ? view.getValues().compressed(unit) : view.getValues().raw(unit);

    DataSelector::Window window = getDataSelectorWindow(showCompressedData, timeline);

    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    ImPlot::HideNextItem(hidden, ImPlotCond_Once);
    ImPlot::PlotLine(style.name, timeline.data() + window.start, values.data() + window.start, static_cast<int>(window.size));
}

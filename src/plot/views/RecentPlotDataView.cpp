#include "RecentPlotDataView.h"

RecentPlotDataView::RecentPlotDataView(const PlotData& data, const PlotStyle& style, size_t timelapseWindow_ms)
    : PlotDataView(data, style), rawDataSelector{timelapseWindow_ms}, compressedDataSelector{timelapseWindow_ms} {
}

DataSelector::Window RecentPlotDataView::getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) {
    RecentDataSelector& selector = getCompressedData ? compressedDataSelector : rawDataSelector;
    return selector.getWindow(timeline);
}

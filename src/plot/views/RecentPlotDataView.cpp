#include "RecentPlotDataView.h"

RecentPlotDataView::RecentPlotDataView(const PlotData& plotData, size_t timelapseWindow_ms)
    : PlotDataView(plotData), rawDataSelector{timelapseWindow_ms}, compressedDataSelector{timelapseWindow_ms} {
}

RecentPlotDataView::RecentPlotDataView(const PlotData& plotData, const ThemedColor& colorOverride, size_t timelapseWindow_ms)
    : PlotDataView(plotData, colorOverride), rawDataSelector{timelapseWindow_ms}, compressedDataSelector{timelapseWindow_ms} {
}

DataSelector::Window RecentPlotDataView::getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) {
    RecentDataSelector& selector = getCompressedData ? compressedDataSelector : rawDataSelector;
    return selector.getWindow(timeline);
}

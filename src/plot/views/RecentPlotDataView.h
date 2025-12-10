#pragma once

#include "DataSelector.h"
#include "PlotDataView.h"
#include "RecentDataSelector.h"

// TODO: FIX BEG WHEN ERASE OLD CAUSES DESYNC ISSUES
class RecentPlotDataView : public PlotDataView {
public:
    RecentPlotDataView(const PlotData& plotData, size_t timelapseWindow_ms);
    RecentPlotDataView(const PlotData& plotData, const ThemedColor& colorOverride, size_t timelapseWindow_ms);

protected:
    DataSelector::Window getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) override;

private:
    RecentDataSelector rawDataSelector;
    RecentDataSelector compressedDataSelector;
};

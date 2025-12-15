#pragma once

#include "DataSelector.h"
#include "PlotStyle.h"

class PlotData;

class PlotDataView {
public:
    PlotDataView(const PlotData& data, const PlotStyle& style);

    void plot(bool showCompressedData);

protected:
    virtual DataSelector::Window getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) = 0;

private:
    const PlotData& data;
    PlotStyle style;
};

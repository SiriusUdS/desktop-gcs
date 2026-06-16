#pragma once

#include "DataSelector.h"
#include "PlotStyle.h"
#include "units.h"

class PlotData;

class PlotDataView {
public:
    PlotDataView(const PlotData& data, const PlotStyle& style);
    virtual ~PlotDataView() = default;

    void plot(Units::Unit time_unit, Units::Unit unit, bool showCompressedData = false, bool hidden = false);

protected:
    virtual DataSelector::Window getDataSelectorWindow(bool getCompressedData, const std::vector<float>& timeline) = 0;

private:
    const PlotData& data;
    PlotStyle style;
};

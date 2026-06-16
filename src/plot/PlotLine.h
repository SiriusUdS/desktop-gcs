#pragma once

#include "PlotStyle.h"
#include "units.h"

class PlotData;

class PlotLine {
public:
    PlotLine(const PlotData& data, const PlotStyle& style);

    void plot(Units::Unit time_unit, Units::Unit unit, bool showCompressed = false, bool hidden = false) const;
    void plot_raw(bool showCompressed = false, bool hidden = false) const;
    const PlotData& getData() const;
    const PlotStyle& getStyle() const;

private:
    const PlotData& data;
    PlotStyle style;
};

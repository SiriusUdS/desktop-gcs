#pragma once

#include "PlotStyle.h"

class PlotData;

class PlotLine {
public:
    PlotLine(const PlotData& data, const PlotStyle& style);

    void plot(bool showCompressed = false) const;
    const PlotData& getData() const;
    const PlotStyle& getStyle() const;

private:
    const PlotData& data;
    PlotStyle style;
};

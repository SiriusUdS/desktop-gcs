#pragma once

#include "PlotLine.h"
#include "PlotStyle.h"
#include "SensorPlotData.h"

struct SensorPlotLine {
    SensorPlotLine(const SensorPlotData& data, PlotStyle style)
        : valuePlotLine{data.getValuePlotData(), style}, adcPlotLine{data.getAdcPlotData(), style} {
    }

    PlotLine valuePlotLine;
    PlotLine adcPlotLine;
};

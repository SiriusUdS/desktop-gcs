#pragma once

#include "ThemedColor.h"
#include "DataSelector.h"
#include "PlotStyle.h"
#include "FullDataSelector.h"
#include "units.h"

#include <memory>

class PlotData;

class PlotLine {
public:
    PlotLine(
        const PlotData& data,
        const PlotStyle& style,
        std::unique_ptr<DataSelector> selector = std::make_unique<FullDataSelector>());

    void plot(
        Units::Unit timeUnit,
        Units::Unit valueUnit,
        bool showCompressed = false,
        bool hidden = false) const;

    const PlotData& getData() const;
    const PlotStyle& getStyle() const;

private:
    const PlotData& data;
    PlotStyle style;

    std::unique_ptr<DataSelector> selector;
};
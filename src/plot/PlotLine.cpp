#include "PlotLine.h"

#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

PlotLine::PlotLine(const PlotData& data, const PlotStyle& style) : data(data), style(style) {
}

void PlotLine::plot(bool showCompressed) const {
    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    if (showCompressed) {
        ImPlot::PlotLine(style.name,
                         data.getTimeline().compressed().data(),
                         data.getValues().compressed().data(),
                         static_cast<int>(data.getValues().compressed().size()));
    } else {
        ImPlot::PlotLine(style.name, data.getTimeline().raw().data(), data.getValues().raw().data(), static_cast<int>(data.getValues().raw().size()));
    }
}

const PlotData& PlotLine::getData() const {
    return data;
}

const PlotStyle& PlotLine::getStyle() const {
    return style;
}

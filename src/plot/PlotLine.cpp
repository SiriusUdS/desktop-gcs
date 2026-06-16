#include "PlotLine.h"

#include "PlotData.h"
#include "ThemedColor.h"

#include <implot.h>

PlotLine::PlotLine(const PlotData& data, const PlotStyle& style) : data(data), style(style) {
}

void PlotLine::plot(Units::Unit time_unit, Units::Unit unit, bool showCompressed, bool hidden) const {
    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    ImPlot::HideNextItem(hidden, ImPlotCond_Once);
    if (showCompressed) {
        ImPlot::PlotLine(style.name,
                         data.getTimeline().compressed(time_unit).data(),
                         data.getValues().compressed(unit).data(),
                         static_cast<int>(data.getValues().compressed().size()));
    } else {
        ImPlot::PlotLine(style.name, data.getTimeline().raw(time_unit).data(), data.getValues().raw(unit).data(), static_cast<int>(data.getValues().raw().size()));
    }
}

void PlotLine::plot_raw(bool showCompressed, bool hidden) const {
    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    ImPlot::HideNextItem(hidden, ImPlotCond_Once);
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

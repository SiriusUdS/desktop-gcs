#include "PlotLine.h"

#include "PlotData.h"

#include <implot.h>

PlotLine::PlotLine(
    const PlotData& data,
    const PlotStyle& style,
    std::unique_ptr<DataSelector> selector)
    :
    data(data),
    style(style),
    selector(std::move(selector))
{
}

void PlotLine::plot(
    Units::Unit timeUnit,
    Units::Unit valueUnit,
    bool showCompressed,
    bool hidden) const
{
    auto lockedView =
        data.makeLockedView();

    const auto timeline =
        showCompressed
            ? lockedView.getTimeline().compressed(timeUnit)
            : lockedView.getTimeline().raw(timeUnit);

    const auto values =
        showCompressed
            ? lockedView.getValues().compressed(valueUnit)
            : lockedView.getValues().raw(valueUnit);

    PlotView view{
        timeline,
        values,
        timeUnit,
        valueUnit
    };

    const auto window =
        selector->getWindow(view);

    ImPlot::SetNextLineStyle(
        style.color.resolve(),
        style.weight);

    ImPlot::HideNextItem(
        hidden,
        ImPlotCond_Once);

    ImPlot::PlotLine(
        style.name,
        timeline.data() + window.start,
        values.data() + window.start,
        static_cast<int>(window.size));
}

const PlotData&
PlotLine::getData() const {
    return data;
}

const PlotStyle&
PlotLine::getStyle() const {
    return style;
}
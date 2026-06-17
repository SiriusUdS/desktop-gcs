#pragma once

#include "DataSelector.h"
#include "units.h"

class RecentDataSelector : public DataSelector {
public:
    RecentDataSelector(float duration, Units::TimeUnit durationUnit);

    Window getWindow(const PlotView& view);

private:
    float duration;
    Units::TimeUnit durationUnit;
    size_t start{};
};
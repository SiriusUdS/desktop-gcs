#pragma once

#include "DataSelector.h"
#include "PlotView.h"

class FullDataSelector : public DataSelector {
public:
    Window getWindow(const PlotView& view) {
        return {0, view.timeline.size()};
    }
};
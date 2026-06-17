#pragma once

#include "PlotView.h"

class DataSelector {
public:
    virtual ~DataSelector() = default;

    struct Window {
        size_t start;
        size_t size;
    };

    virtual Window getWindow(const PlotView& view) = 0;
};
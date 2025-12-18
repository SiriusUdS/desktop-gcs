#pragma once

#include "PlotDataUpdateListener.h"

#include <vector>

class DataSelector : public PlotDataUpdateListener {
public:
    DataSelector(const PlotData* data) : PlotDataUpdateListener({data}) {
    }

    struct Window {
        size_t start;
        size_t size;
    };

public:
    virtual Window getWindow(const std::vector<float>& data) = 0;
};

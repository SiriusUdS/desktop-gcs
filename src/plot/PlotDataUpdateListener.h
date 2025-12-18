#pragma once

#include "PlotData.h"

class PlotDataUpdateListener {
public:
    PlotDataUpdateListener(std::vector<const PlotData*> dataVec) : dv(dataVec) {
    }

    void subscribe() {
        for (const PlotData* data : dv) {
            data->addListener(this);
        }
    }

    virtual void onAddData(const PlotData* plotData, float timestamp, float value) {
    }
    virtual void onClear(const PlotData* plotData) {
    }
    virtual void onCompress(const PlotData* plotData) {
    }
    virtual void onEraseOld(const PlotData* plotData) {
    }

private:
    std::vector<const PlotData*> dv;
};

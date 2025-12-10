#pragma once

class PlotData;

class PlotDataUpdateListener {
public:
    virtual void onSubscribe(const PlotData* plotData) = 0;
    virtual void onAddData(const PlotData* plotData, float timestamp, float value) = 0;
};

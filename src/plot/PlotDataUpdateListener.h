#pragma once

class PlotData;

// TODO: SUBSCRIBE TO PLOTDATA WHEN OBJECT CREATED (CONSTRUCTOR)
class PlotDataUpdateListener {
public:
    virtual void onSubscribe(const PlotData* plotData) {
    }
    virtual void onAddData(const PlotData* plotData, float timestamp, float value) {
    }
    virtual void onClear(const PlotData* plotData) {
    }
    virtual void onCompress(const PlotData* plotData) {
    }
    virtual void onEraseOld(const PlotData* plotData) {
    }
};

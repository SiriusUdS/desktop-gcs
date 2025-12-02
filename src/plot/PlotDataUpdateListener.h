#ifndef PLOTDATAUPDATELISTENER_H
#define PLOTDATAUPDATELISTENER_H

class PlotData;

class PlotDataUpdateListener {
public:
    virtual void onSubscribe(const PlotData* plotData) = 0;
    virtual void onAddData(const PlotData* plotData, float timestamp, float value) = 0;
};

#endif // PLOTDATAUPDATELISTENER_H

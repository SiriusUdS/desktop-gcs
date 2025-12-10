#pragma once

#include "PlotDataUpdateListener.h"

#include <map>
#include <mutex>
#include <vector>

class TankMassPlotDataUpdater : public PlotDataUpdateListener {
public:
    void onSubscribe(const PlotData* plotData) override;
    void onAddData(const PlotData* plotData, float timestamp, float value) override;

private:
    struct PlotDataUpdate {
        float timestamp{};
        float value{};
        bool wasUpdated{};
        std::chrono::steady_clock::time_point lastUpdateTime{};
    };

    std::map<const PlotData*, PlotDataUpdate> plotDataUpdateMap;
    std::mutex mtx;
};

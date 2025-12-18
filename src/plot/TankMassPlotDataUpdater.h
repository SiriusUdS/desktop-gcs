#pragma once

#include "PlotDataUpdateListener.h"

#include <map>
#include <mutex>
#include <vector>

class TankMassPlotDataUpdater : public PlotDataUpdateListener {
public:
    TankMassPlotDataUpdater(std::vector<const PlotData*> dataVec);
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

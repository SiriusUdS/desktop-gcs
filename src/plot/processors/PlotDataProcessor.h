#pragma once

#include "PlotDataUpdateListener.h"

#include <map>
#include <mutex>
#include <vector>

class PlotDataProcessor : public PlotDataUpdateListener {
public:
    PlotDataProcessor(std::vector<const PlotData*> dataVec);
    virtual ~PlotDataProcessor() = default;

    void onAddData(const PlotData* plotData, float timestamp, float value) override;

protected:
    struct PlotDataProcessData {
        float timestamp{};
        float value{};
        bool wasUpdated{};
        std::chrono::steady_clock::time_point lastUpdateTime{};
    };

    virtual void processNewData() = 0;

    std::map<const PlotData*, PlotDataProcessData> plotDataUpdateMap;
    std::mutex mtx;
};

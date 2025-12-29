#include "PlotDataProcessor.h"

#include "Logging.h"
#include "PlotDataUpdateListener.h"

PlotDataProcessor::PlotDataProcessor(std::vector<const PlotData*> dataVec) : PlotDataUpdateListener(dataVec) {
    for (const PlotData* data : dataVec) {
        plotDataUpdateMap.emplace(data, PlotDataProcessData());
    }
}

void PlotDataProcessor::onAddData(const PlotData* plotData, float timestamp, float value) {
    std::lock_guard<std::mutex> lock(mtx);

    auto updatedSourceIt = plotDataUpdateMap.find(plotData);
    if (updatedSourceIt == plotDataUpdateMap.end()) {
        GCS_APP_LOG_ERROR("PlotDataProcessor: Notifier plot data not found in tracked plot data map.");
        return;
    }
    PlotDataProcessData& updatedSource = updatedSourceIt->second;
    updatedSource.timestamp = timestamp;
    updatedSource.value = value;
    updatedSource.wasUpdated = true;
    updatedSource.lastUpdateTime = std::chrono::steady_clock::now();

    // Check if all tracked plot data where updated since last time a plot data point was computed
    // Also make sure the updates are recent enough
    constexpr auto MAX_TIME_DIFF = std::chrono::milliseconds(100000); // TODO: Make this lower in the future when not doing tests anymore
    bool allPlotDataUpdated = true;
    auto newest = std::chrono::steady_clock::time_point::min();
    auto oldest = std::chrono::steady_clock::time_point::max();

    for (const auto& [_, update] : plotDataUpdateMap) {
        if (!update.wasUpdated) {
            allPlotDataUpdated = false;
            break;
        }
        newest = std::max(newest, update.lastUpdateTime);
        oldest = std::min(oldest, update.lastUpdateTime);
    }

    if (!allPlotDataUpdated) {
        return;
    }

    if (newest - oldest > MAX_TIME_DIFF) {
        GCS_APP_LOG_DEBUG("PlotDataProcessor: Skipped update due to desynchronization ({}ms)",
                          std::chrono::duration_cast<std::chrono::milliseconds>(newest - oldest).count());
        return;
    }

    processNewData();

    // Mark all plot data update as not recently updated
    for (auto& [_, update] : plotDataUpdateMap) {
        update.wasUpdated = false;
    }
}

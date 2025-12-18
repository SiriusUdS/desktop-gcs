#pragma once

#include "DataSeries.h"

#include <mutex>

class PlotDataUpdateListener;

class PlotData {
public:
    class LockedView {
    public:
        LockedView(std::mutex& mtx, const DataSeries& timeline, const DataSeries& values);
        const DataSeries& getTimeline() const;
        const DataSeries& getValues() const;

    private:
        const std::lock_guard<std::mutex> lock;
        const DataSeries& timeline;
        const DataSeries& values;
    };

public:
    PlotData();
    void addData(float timestamp, float value);
    void clear();
    void addListener(PlotDataUpdateListener* listener) const;
    float recentAverageValue(size_t duration_ms) const;
    const DataSeries& getTimeline() const;
    const DataSeries& getValues() const;
    size_t getSize() const;
    LockedView makeLockedView() const;

private:
    static constexpr size_t MAX_ORIGINAL_DATA_SIZE = 100'000;
    static constexpr size_t DATA_AMOUNT_TO_DROP_IF_MAX_REACHED = 10'000;
    static constexpr size_t MAX_COMPRESSED_DATA_SIZE = 20'000;
    static constexpr size_t TARGET_COMPRESSED_DATA_SIZE = 10'000;

    DataSeries timeline;
    DataSeries values;
    mutable std::vector<PlotDataUpdateListener*> listeners;
    mutable std::mutex mtx;
};

#ifndef PLOTDATA_H
#define PLOTDATA_H

#include "DataSeries.h"
#include "PlotStyle.h"

#include <mutex>

class PlotDataUpdateListener;

class PlotData {
public:
    class LockedView {
    public:
        LockedView(std::mutex& mtx, const DataSeries& timeline, const DataSeries& values, const PlotStyle& style);
        const DataSeries& getTimeline() const;
        const DataSeries& getValues() const;
        const PlotStyle& getStyle() const;

    private:
        const std::lock_guard<std::mutex> lock;
        const DataSeries& timeline;
        const DataSeries& values;
        const PlotStyle& style;
    };

public:
    PlotData(const char* name, const ThemedColor& color);
    void addData(float timestamp, float value);
    void clear();
    void addListener(PlotDataUpdateListener* listener);
    void plot(bool showCompressedData) const;
    float recentAverageValue(size_t duration_ms) const;
    const char* getName() const;
    const ThemedColor& getColor() const;
    LockedView makeLockedView() const;

private:
    static constexpr size_t MAX_ORIGINAL_DATA_SIZE = 100'000;
    static constexpr size_t DATA_AMOUNT_TO_DROP_IF_MAX_REACHED = 10'000;
    static constexpr size_t MAX_COMPRESSED_DATA_SIZE = 20'000;
    static constexpr size_t TARGET_COMPRESSED_DATA_SIZE = 10'000;

    static constexpr float DEFAULT_PLOT_LINE_THICKNESS = 4;

    DataSeries timeline;
    DataSeries values;
    PlotStyle style;
    std::vector<PlotDataUpdateListener*> listeners;
    mutable std::mutex mtx;
};

#endif // PLOTDATA_H

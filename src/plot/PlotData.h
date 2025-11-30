#ifndef PLOTDATA_H
#define PLOTDATA_H

#include "PlotRawData.h"
#include "PlotStyle.h"

#include <mutex>

class PlotDataUpdateListener;
struct ThemedColor;

/**
 * @class PlotData
 * @brief Represents data of a single plot line to be shown on an ImPlot ImGui component
 */
class PlotData {
public:
    class LockedView {
    public:
        LockedView(std::mutex& mtx, const PlotRawData& data, const PlotRawData& compressedData, const PlotStyle& style);
        const PlotRawData& getData() const;
        const PlotRawData& getCompressedData() const;
        const PlotStyle& getStyle() const;

    private:
        const std::lock_guard<std::mutex> lock;
        const PlotRawData& data;
        const PlotRawData& compressedData;
        const PlotStyle& style;
    };

public:
    PlotData(const char* name, const ThemedColor& color);
    void addData(float x, float y);
    void clear();
    void addListener(PlotDataUpdateListener* listener);
    void plot(bool showCompressedData) const;
    float recentAverageValue(size_t durationMs) const;
    const char* getName() const;
    const ThemedColor& getColor() const;
    LockedView makeLockedView() const;

private:
    void clearImpl();
    void dropOldData(size_t amount);

    static constexpr size_t MAX_ORIGINAL_DATA_SIZE = 100'000;
    static constexpr size_t DATA_AMOUNT_TO_DROP_IF_MAX_REACHED = 10'000;
    static constexpr size_t MAX_COMPRESSED_DATA_SIZE = 20'000;
    static constexpr size_t TARGET_COMPRESSED_DATA_SIZE = 10'000;

    static constexpr float DEFAULT_PLOT_LINE_THICKNESS = 4;

    PlotRawData data;
    PlotRawData compressedData;
    PlotStyle style;
    std::vector<PlotDataUpdateListener*> listeners;
    mutable std::mutex mtx;
};

#endif // PLOTDATA_H

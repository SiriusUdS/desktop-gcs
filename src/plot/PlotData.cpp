#include "PlotData.h"

#include "Logging.h"
#include "PlotColors.h"
#include "PlotDataCompression.h"
#include "PlotDataUpdateListener.h"

#include <implot.h>

PlotData::LockedView::LockedView(std::mutex& mtx, const PlotRawData& data, const PlotRawData& compressedData, const PlotStyle& style)
    : lock(mtx), data(data), compressedData(compressedData), style(style) {
}

const PlotRawData& PlotData::LockedView::getData() const {
    return data;
}

const PlotRawData& PlotData::LockedView::getCompressedData() const {
    return compressedData;
}

const PlotStyle& PlotData::LockedView::getStyle() const {
    return style;
}

/**
 * @brief Constructs a plot data from a name and a color of the plot data line
 * @param n Name of the plot data
 * @param c Color of the plot data line
 */
PlotData::PlotData(const char* n, ImVec4 c) : style{n, c, 2} { // TODO: Put this "2" in a constant somewhere
}

/**
 * @brief Adds a single data point to the plot data.
 * @param x Where the data is placed on the X axis.
 * @param y Where the data is placed on the Y axis.
 */
void PlotData::addData(float x, float y) {
    std::lock_guard<std::mutex> lock(mtx);

    if (data.size() && x < data.lastX()) {
        GCS_APP_LOG_WARN("PlotData: Received unordered data for plot data {}, clearing data.", style.name);
        // TODO: Figure out how to make clear() thread-safe without causing a deadlock
        clear();
    }

    data.add(x, y);
    compressedData.add(x, y);

    for (PlotDataUpdateListener* listener : listeners) {
        listener->onAddData(this, x, y);
    }

    if (data.size() > MAX_ORIGINAL_DATA_SIZE) {
        dropOldData(DATA_AMOUNT_TO_DROP_IF_MAX_REACHED);
    }

    if (compressedData.size() > MAX_COMPRESSED_DATA_SIZE) {
        PlotDataCompression::meanCompression(data, compressedData, TARGET_COMPRESSED_DATA_SIZE, style.name);
    }
}

/**
 * @brief Clears the raw and the compressed data.
 */
void PlotData::clear() {
    // TODO - Uncomment this later
    // std::lock_guard<std::mutex> lock(mtx);

    data.clear();
    compressedData.clear();
}

void PlotData::addListener(PlotDataUpdateListener* listener) {
    std::lock_guard<std::mutex> lock(mtx);

    listeners.push_back(listener);
    listener->onSubscribe(this);
}

/**
 * @brief Display the plot line. This should be called after a "ImPlot::BeginPlot" call.
 * @param showCompressedData Plot compressed data to improve performances
 */
void PlotData::plot(bool showCompressedData) const {
    std::lock_guard<std::mutex> lock(mtx);

    ImPlot::SetNextLineStyle(style.color, style.weight);
    const PlotRawData& dataToPlot = showCompressedData ? compressedData : data;
    ImPlot::PlotLine(style.name, dataToPlot.getRawX(), dataToPlot.getRawY(), static_cast<int>(dataToPlot.size()));
}

/**
 * @brief Compute the average value of the data in the last x milliseconds.
 * @param durationMs The duration in milliseconds in which we measure the recent average value.
 * @returns The recent average value.
 */
float PlotData::recentAverageValue(size_t durationMs) const {
    std::lock_guard<std::mutex> lock(mtx);

    const size_t dataSize = data.size();

    if (dataSize == 0) {
        return 0.f;
    }

    const float lastTimestamp = data.lastX();
    float sum = 0.f;
    size_t count = 0;

    for (size_t i = dataSize; i-- > 0;) {
        if ((lastTimestamp - data.getXAt(i)) > durationMs) {
            break;
        }
        sum += data.getYAt(i);
        count++;
    }

    if (count == 0) {
        return 0.f;
    }

    return sum / count;
}

/**
 * @brief Returns the name of the plot data.
 * @returns The name of the plot data.
 */
const char* PlotData::getName() const {
    // TODO - Uncomment this later
    // std::lock_guard<std::mutex> lock(mtx);

    return style.name;
}

PlotData::LockedView PlotData::makeLockedView() const {
    return LockedView(mtx, data, compressedData, style);
}

/**
 * @brief Drops a fixed amount of data points, starting from the oldest data.
 * @param amount Amount of data to drop.
 */
void PlotData::dropOldData(size_t amount) {
    size_t oldDataSize = data.size();

    if (oldDataSize < amount) {
        GCS_APP_LOG_DEBUG("PlotData: Dropping old data unnecessary for plot data {}, current size {} smaller than amount to drop {}.",
                          style.name,
                          oldDataSize,
                          amount);
        return;
    }

    data.eraseOld(amount);
    PlotDataCompression::meanCompression(data, compressedData, TARGET_COMPRESSED_DATA_SIZE, style.name);

    GCS_APP_LOG_DEBUG("PlotData: Plot data {} successfully dropped old data, went from size {} to {}.", style.name, oldDataSize, data.size());
}

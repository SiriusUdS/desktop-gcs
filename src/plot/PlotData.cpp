#include "PlotData.h"

#include "Logging.h"
#include "PlotDataUpdateListener.h"
#include "ThemedColor.h"

#include <implot.h>

PlotData::LockedView::LockedView(std::mutex& mtx, const DataSeries& timeline, const DataSeries& values, const PlotStyle& style)
    : lock(mtx), timeline(timeline), values(values), style(style) {
}

const DataSeries& PlotData::LockedView::getTimeline() const {
    return timeline;
}

const DataSeries& PlotData::LockedView::getValues() const {
    return values;
}

const PlotStyle& PlotData::LockedView::getStyle() const {
    return style;
}

/**
 * @brief Constructs a plot data from a name and a color of the plot data line
 * @param name Name of the plot data
 * @param color Color of the plot data line
 */
PlotData::PlotData(const char* name, const ThemedColor& color)
    : style{name, color, DEFAULT_PLOT_LINE_THICKNESS}, timeline(TARGET_COMPRESSED_DATA_SIZE), values(TARGET_COMPRESSED_DATA_SIZE) {
}

/**
 * @brief Adds a single data point to the plot data.
 * @param timestamp Where the data is placed on the X axis.
 * @param value Where the data is placed on the Y axis.
 */
void PlotData::addData(float timestamp, float value) {
    std::lock_guard<std::mutex> lock(mtx);

    if (timeline.raw().size() && timestamp < timeline.raw().back()) {
        GCS_APP_LOG_WARN("PlotData: Received unordered data for plot data {}, clearing data.", style.name);
        timeline.clear();
        values.clear();
    }

    timeline.add(timestamp);
    values.add(value);

    for (PlotDataUpdateListener* listener : listeners) {
        listener->onAddData(this, timestamp, value);
    }

    if (values.raw().size() > MAX_ORIGINAL_DATA_SIZE) {
        timeline.eraseOld(DATA_AMOUNT_TO_DROP_IF_MAX_REACHED);
        values.eraseOld(DATA_AMOUNT_TO_DROP_IF_MAX_REACHED);
    }

    if (values.compressed().size() > MAX_COMPRESSED_DATA_SIZE) {
        timeline.compress();
        values.compress();
    }
}

/**
 * @brief Clears the raw and compressed data.
 */
void PlotData::clear() {
    std::lock_guard<std::mutex> lock(mtx);

    timeline.clear();
    values.clear();
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

    ImPlot::SetNextLineStyle(style.color.resolve(), style.weight);
    if (showCompressedData) {
        ImPlot::PlotLine(style.name, timeline.compressed().data(), values.compressed().data(), static_cast<int>(values.compressed().size()));
    } else {
        ImPlot::PlotLine(style.name, timeline.raw().data(), values.raw().data(), static_cast<int>(values.raw().size()));
    }
}

/**
 * @brief Compute the average value of the data in the last x milliseconds.
 * @param duration_ms The duration in milliseconds in which we measure the recent average value.
 * @returns The recent average value.
 */
float PlotData::recentAverageValue(size_t duration_ms) const {
    std::lock_guard<std::mutex> lock(mtx);

    const size_t dataSize = values.raw().size();

    if (dataSize == 0) {
        return 0.f;
    }

    const float lastTimestamp = timeline.raw().back();
    float sum = 0.f;
    size_t count = 0;

    for (size_t i = dataSize; i-- > 0;) {
        if ((lastTimestamp - timeline.raw().at(i)) > duration_ms) {
            break;
        }
        sum += values.raw().at(i);
        count++;
    }

    if (count == 0) {
        return 0.f;
    }

    return sum / count;
}

float PlotData::latestTimestamp() const {
    std::lock_guard<std::mutex> lock(mtx);

    return timeline.raw().back();
}

float PlotData::latestValue() const {
    std::lock_guard<std::mutex> lock(mtx);

    return values.raw().back();
}

size_t PlotData::getSize() const {
    std::lock_guard<std::mutex> lock(mtx);

    return timeline.raw().size();
}

/**
 * @brief Returns the name of the plot data.
 * @returns The name of the plot data.
 */
const char* PlotData::getName() const {
    std::lock_guard<std::mutex> lock(mtx);

    return style.name;
}

/**
 * @brief Returns the color of the plot line.
 * @returns The color of the plot line.
 */
const ThemedColor& PlotData::getColor() const {
    std::lock_guard<std::mutex> lock(mtx);

    return style.color;
}

PlotData::LockedView PlotData::makeLockedView() const {
    return LockedView(mtx, timeline, values, style);
}

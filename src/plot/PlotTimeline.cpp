#include "PlotTimeline.h"

#include "PlotDataCompression.h"

PlotTimeline::PlotTimeline(size_t targetCompressionSize) : DataSeries(targetCompressionSize) {
}

void PlotTimeline::add(float timestamp) {
    if (values.size() && timestamp < values.back()) {
        dataUnordered = true;
    }
    values.push_back(timestamp);
    compressedValues.push_back(timestamp);
}

void PlotTimeline::clear() {
    values.clear();
    compressedValues.clear();
    dataUnordered = false;
}

bool PlotTimeline::isUnordered() const {
    return dataUnordered;
}

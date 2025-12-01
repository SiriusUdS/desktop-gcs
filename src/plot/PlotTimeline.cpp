#include "PlotTimeline.h"

#include "PlotDataCompression.h"

void PlotTimeline::add(float timestamp) {
    if (values.size() && timestamp < values.back()) {
        dataUnordered = true;
    }
    values.push_back(timestamp);

    if (values.size() > MAX_ORIGINAL_DATA_SIZE) {
        eraseOld(DATA_AMOUNT_TO_DROP_IF_MAX_REACHED);
    }

    if (values.size() > MAX_COMPRESSED_DATA_SIZE) {
        PlotDataCompression::meanCompression(values, compressedValues, TARGET_COMPRESSED_DATA_SIZE);
    }
}

void PlotTimeline::clear() {
    values.clear();
    compressedValues.clear();
    dataUnordered = false;
}

bool PlotTimeline::isUnordered() const {
    return dataUnordered;
}

#include "PlotValues.h"

#include "PlotDataCompression.h"

void PlotValues::add(float value) {
    values.push_back(value);

    if (values.size() > MAX_ORIGINAL_DATA_SIZE) {
        eraseOld(DATA_AMOUNT_TO_DROP_IF_MAX_REACHED);
    }

    if (values.size() > MAX_COMPRESSED_DATA_SIZE) {
        PlotDataCompression::meanCompression(values, compressedValues, TARGET_COMPRESSED_DATA_SIZE);
    }
}

void PlotValues::clear() {
    values.clear();
    compressedValues.clear();
}

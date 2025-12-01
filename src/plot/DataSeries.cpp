#include "DataSeries.h"

#include "PlotDataCompression.h"

void DataSeries::eraseOld(size_t count) {
    values.erase(values.begin(), values.begin() + count);
    PlotDataCompression::meanCompression(values, compressedValues, TARGET_COMPRESSED_DATA_SIZE);
}

size_t DataSeries::size() const {
    return values.size();
}

float DataSeries::last() const {
    return values.back();
}

float DataSeries::at(size_t index) const {
    return values.at(index);
}

const float* DataSeries::data() const {
    return values.data();
}

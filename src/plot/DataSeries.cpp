#include "DataSeries.h"

#include "PlotDataCompression.h"

DataSeries::DataSeries(size_t targetCompressionSize) : targetCompressionSize(targetCompressionSize) {
}

void DataSeries::compress() {
    PlotDataCompression::meanCompression(values, compressedValues, targetCompressionSize);
}

void DataSeries::eraseOld(size_t count) {
    values.erase(values.begin(), values.begin() + count);
    compress();
}

size_t DataSeries::size() const {
    return values.size();
}

size_t DataSeries::compressedSize() const {
    return compressedValues.size();
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

const float* DataSeries::compressedData() const {
    return compressedValues.data();
}

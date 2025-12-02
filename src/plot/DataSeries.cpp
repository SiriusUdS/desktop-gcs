#include "DataSeries.h"

#include "PlotDataCompression.h"

DataSeries::DataSeries(size_t targetCompressionSize) : targetCompressionSize(targetCompressionSize) {
}

void DataSeries::add(float value) {
    values.push_back(value);
    compressedValues.push_back(value);
}

void DataSeries::clear() {
    values.clear();
    compressedValues.clear();
}

void DataSeries::compress() {
    PlotDataCompression::meanCompression(values, compressedValues, targetCompressionSize);
}

void DataSeries::eraseOld(size_t count) {
    values.erase(values.begin(), values.begin() + count);
    compress();
}

const std::vector<float>& DataSeries::raw() const {
    return values;
}

const std::vector<float>& DataSeries::compressed() const {
    return compressedValues;
}

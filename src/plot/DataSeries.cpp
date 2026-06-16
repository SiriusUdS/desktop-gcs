#include "DataSeries.h"

#include "PlotDataCompression.h"

#include <algorithm>

DataSeries::DataSeries(size_t targetCompressionSize, Units::Unit unit) {
    this->targetCompressionSize = targetCompressionSize;
    this->unit = unit;
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

Units::Unit DataSeries::getUnit() {
    return unit;
}

std::vector<float> DataSeries::raw() const {
    return values;
}

std::vector<float> DataSeries::raw(Units::Unit unit) const {
    if (unit == this->unit) {
        return values;
    }

    std::vector<float> converted(values.size());
    std::transform(values.begin(), values.end(), converted.begin(), [this, unit](float value) { return Units::convert(value, this->unit, unit); });
    return converted;
}

std::vector<float> DataSeries::compressed() const {
    return compressedValues;
}

std::vector<float> DataSeries::compressed(Units::Unit unit) const {
    if (unit == this->unit) {
        return compressedValues;
    }

    std::vector<float> converted(compressedValues.size());
    std::transform(compressedValues.begin(), compressedValues.end(), converted.begin(), [this, unit](float value) { return Units::convert(value, this->unit, unit); });
    return converted;
}

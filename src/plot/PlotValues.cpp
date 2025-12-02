#include "PlotValues.h"

#include "PlotDataCompression.h"

PlotValues::PlotValues(size_t targetCompressionSize) : DataSeries(targetCompressionSize) {
}

void PlotValues::add(float value) {
    values.push_back(value);
    compressedValues.push_back(value);
}

void PlotValues::clear() {
    values.clear();
    compressedValues.clear();
}

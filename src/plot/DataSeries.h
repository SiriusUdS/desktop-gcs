#pragma once

#include <vector>
#include "units.h"

class DataSeries {
public:
    DataSeries(size_t targetCompressionSize, Units::Unit unit);
    void add(float value);
    void clear();
    void compress();
    void eraseOld(size_t count);
    Units::Unit getUnit();

    std::vector<float> raw() const;
    std::vector<float> raw(Units::Unit unit) const;
    std::vector<float> compressed() const;
    std::vector<float> compressed(Units::Unit unit) const;

protected:
    Units::Unit unit;
    std::vector<float> values;
    std::vector<float> compressedValues;
    size_t targetCompressionSize{};
};

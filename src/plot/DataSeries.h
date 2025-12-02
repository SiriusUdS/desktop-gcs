#ifndef DATASERIES_H
#define DATASERIES_H

#include <vector>

class DataSeries {
public:
    DataSeries(size_t targetCompressionSize);
    void add(float value);
    void clear();
    void compress();
    void eraseOld(size_t count);

    const std::vector<float>& raw() const;
    const std::vector<float>& compressed() const;

protected:
    std::vector<float> values;
    std::vector<float> compressedValues;
    size_t targetCompressionSize{};
};

#endif // DATASERIES_H

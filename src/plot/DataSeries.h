#ifndef DATASERIES_H
#define DATASERIES_H

#include <vector>

class DataSeries {
public:
    DataSeries(size_t targetCompressionSize);

    virtual void add(float value) = 0;
    virtual void clear() = 0;

    void compress();
    void eraseOld(size_t count);
    size_t size() const;
    size_t compressedSize() const;
    float last() const;
    float at(size_t index) const;
    const float* data() const;
    const float* compressedData() const;

protected:
    std::vector<float> values;
    std::vector<float> compressedValues;
    size_t targetCompressionSize{};
};

#endif // DATASERIES_H

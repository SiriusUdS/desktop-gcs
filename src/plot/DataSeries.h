#ifndef DATASERIES_H
#define DATASERIES_H

#include <vector>

class DataSeries {
public:
    virtual void add(float value) = 0;
    virtual void clear() = 0;

    void eraseOld(size_t count);
    size_t size() const;
    float last() const;
    float at(size_t index) const;
    const float* data() const;

protected:
    std::vector<float> values;
    std::vector<float> compressedValues;

    static constexpr size_t MAX_COMPRESSED_DATA_SIZE = 20'000;
    static constexpr size_t TARGET_COMPRESSED_DATA_SIZE = 10'000;
    static constexpr size_t MAX_ORIGINAL_DATA_SIZE = 100'000;
    static constexpr size_t DATA_AMOUNT_TO_DROP_IF_MAX_REACHED = 10'000;
};

#endif // DATASERIES_H

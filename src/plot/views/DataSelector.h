#ifndef DATASELECTOR_H
#define DATASELECTOR_H

#include <vector>

class DataSelector {
public:
    struct Window {
        size_t start;
        size_t size;
    };

public:
    virtual Window getWindow(const std::vector<float>& data) = 0;
};

#endif // DATASELECTOR_H

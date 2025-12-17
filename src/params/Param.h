#pragma once

#include <atomic>
#include <string>

template <typename T>
struct Param {
    Param(T defaultValue, std::string iniKey) : currentValue{defaultValue}, defaultValue{defaultValue}, iniKey{iniKey} {
    }

    void reset() {
        currentValue = defaultValue.load();
    }

    std::atomic<T> currentValue;
    std::atomic<T> defaultValue;
    std::string iniKey;
};

using FloatParam = Param<float>;

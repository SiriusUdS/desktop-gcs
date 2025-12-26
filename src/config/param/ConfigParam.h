#pragma once

#include <atomic>
#include <string>

template <typename T>
struct ConfigParam {
    ConfigParam(T defaultValue, std::string iniKey) : currentValue{defaultValue}, defaultValue{defaultValue}, iniKey{iniKey} {
    }

    void reset() {
        currentValue = defaultValue.load();
    }

    std::atomic<T> currentValue;
    std::atomic<T> defaultValue;
    std::string iniKey;
};

using FloatConfigParam = ConfigParam<float>;

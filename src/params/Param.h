#pragma once

#include <string>

template <typename T>
struct Param {
    Param(T defaultValue, std::string iniKey) : currentValue{defaultValue}, defaultValue{defaultValue}, iniKey{iniKey} {
    }

    T currentValue;
    T defaultValue;
    std::string iniKey;
};

// TODO: std::atomic<float>
using FloatParam = Param<float>;

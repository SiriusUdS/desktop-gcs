#pragma once
#include <cstdint>

namespace Thermocouple {
    float ADC_to_Celcius(int32_t ADC) { return static_cast<float>(ADC) / 128.0f; }
}
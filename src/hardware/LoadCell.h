#pragma once
#include <cstdint>

namespace LoadCell {
    float ADC_to_Pounds(int32_t ADC, float max_lbs, float V_experimental, float gain) {
        float volt = ADC * 1.2f / (float((1 << 23) - 1) * gain);
        return volt * max_lbs / (2.9959f * V_experimental);
    }

    namespace LC103B_5K {
        float ADC_to_Pounds(int32_t ADC, float V_experimental, float gain) {
            return LoadCell::ADC_to_Pounds(ADC, 5000, V_experimental, gain);
        }
    }

    namespace LC103B_200 {
        float ADC_to_Pounds(int32_t ADC, float V_experimental, float gain) {
            return LoadCell::ADC_to_Pounds(ADC, 200, V_experimental, gain);
        }
    }

    //64
}
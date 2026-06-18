#pragma once
#include <cstdint>

namespace PressureTransducer {
    float ADC_to_Psi(int32_t ADC, float pressure_full_scale, float voltage_full_scale, float V_zero, float gain) {
        float volt = ADC * 1.2f / (float((1 << 23) - 1) * gain);
        return pressure_full_scale * (volt - V_zero) / (voltage_full_scale - V_zero);
    } // TODO Implement

    namespace Unknown {
        float ADC_to_Psi(int32_t ADC, float V_zero, float gain) {
            return PressureTransducer::ADC_to_Psi(ADC, 1000, 0.1, V_zero, gain);
        }
    }
}
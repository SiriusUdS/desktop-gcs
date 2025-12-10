#pragma once

#include <stdint.h>

namespace PressureTransducer {
float adcToPressure_psi(float adcValue, uint16_t sensorIndex);
} // namespace PressureTransducer

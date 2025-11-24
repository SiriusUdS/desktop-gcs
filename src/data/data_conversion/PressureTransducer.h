#ifndef PRESSURETRANSDUCER_H
#define PRESSURETRANSDUCER_H

#include <stdint.h>

namespace PressureTransducer {
float adcToPressure_psi(float adcValue, uint16_t sensorIndex);
} // namespace PressureTransducer

#endif // PRESSURETRANSDUCER_H

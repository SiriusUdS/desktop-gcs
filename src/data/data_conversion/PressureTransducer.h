#ifndef PRESSURETRANSDUCER_H
#define PRESSURETRANSDUCER_H

#include <stdint.h>

namespace PressureTransducer {
float adcToPressure(float adcValue, uint16_t sensorIndex);
} // namespace PressureTransducer

#endif // PRESSURETRANSDUCER_H

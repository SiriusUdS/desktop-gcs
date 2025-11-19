#include "PressureTransducer.h"

#include "Logging.h"
#include "PlotConfig.h"

#include <algorithm>

namespace PressureTransducer {
struct PressureTransducerParams {
    double additiveFactor{};
    double functionRateOfChange{};
    double functionOffset{};
};

constexpr size_t PRESSURE_TRANSDUCER_AMOUNT = 4;

// clang-format off
constexpr PressureTransducerParams PRESSURE_TRANSDUCER_PARAMS_TABLE[PRESSURE_TRANSDUCER_AMOUNT] = {
    {.additiveFactor=-158.37, .functionRateOfChange=0.9767, .functionOffset=0.0},  // Filling Station - IPA
    {.additiveFactor=-442.65, .functionRateOfChange=0.9414, .functionOffset=0.0}, // Filling Station - NOS
    {.additiveFactor=43.0, .functionRateOfChange=0.9438, .functionOffset=0.0}, // Engine - Chamber
	{.additiveFactor=-43.0, .functionRateOfChange=0.9202, .functionOffset=0.0}  // Engine - Tank
    // OLD {.additiveFactor=-55.316, .functionRateOfChange=0.9438, .functionOffset=0.0}, // Engine - Chamber
	// OLD {.additiveFactor=-717.67, .functionRateOfChange=0.9202, .functionOffset=0.0}  // Engine - Tank
};
// clang-format on
} // namespace PressureTransducer

float PressureTransducer::adcToPressure(float adcValue, uint16_t sensorIndex) {
    const PressureTransducerParams& params = PRESSURE_TRANSDUCER_PARAMS_TABLE[sensorIndex];
    const float pressure_psi =
      static_cast<float>(params.functionRateOfChange * adcValue) - static_cast<float>(params.functionOffset - params.additiveFactor);
    return std::max(pressure_psi, 0.f);
}
/*float PressureTransducer::adcToPressure(float adcValue, uint16_t sensorIndex) {
    static constexpr float PRESSURE_SENSOR_VOLTAGE_RANGE_V = 3.3f;
    static constexpr float PRESSURE_SENSOR_ADC_RANGE_BIT = 4095.f;

    static constexpr double PRESSURE_SENSOR_29673136_SLOPE = 1126.8605003120L;
    static constexpr double PRESSURE_SENSOR_29673136_CONST = -32.4928073293L;
    static constexpr double PRESSURE_SENSOR_29685373_SLOPE = 1158.2499813656L;
    static constexpr double PRESSURE_SENSOR_29685373_CONST = 56.8114389484L;
    static constexpr double PRESSURE_SENSOR_310989_SLOPE = 1164.9569321944L;
    static constexpr double PRESSURE_SENSOR_310989_CONST = -79.7249315302L;
    static constexpr double PRESSURE_SENSOR_3109815_SLOPE = 1160.5427492304L;
    static constexpr double PRESSURE_SENSOR_3109815_CONST = -107.2803695064L;

    static constexpr size_t PRESSURE_SENSOR_COUNT = 4;
    static constexpr double PRESSURE_SENSOR_ARRAY_SLOPE[PRESSURE_SENSOR_COUNT] = {PRESSURE_SENSOR_29673136_SLOPE, PRESSURE_SENSOR_29685373_SLOPE,
                                                                                  PRESSURE_SENSOR_310989_SLOPE, PRESSURE_SENSOR_3109815_SLOPE};
    static constexpr double PRESSURE_SENSOR_ARRAY_CONST[PRESSURE_SENSOR_COUNT] = {PRESSURE_SENSOR_29673136_CONST, PRESSURE_SENSOR_29685373_CONST,
                                                                                  PRESSURE_SENSOR_310989_CONST, PRESSURE_SENSOR_3109815_CONST};

    static constexpr float ADC_MIN_PRESSURE = 0;
    static constexpr float ADC_MAX_PRESSURE = 4095; // TODO: Remove if redundant

    if (sensorIndex >= PRESSURE_SENSOR_COUNT) {
        // TODO: UNCOMMENT LATER
        // GCS_LOG_ERROR("PressureTransducer: Sensor index out of range.");
        return PlotConfig::INVALID_VALUE;
    }

    // TODO: UNCOMMENT LATER
    // else if (adcValue < ADC_MIN_PRESSURE || adcValue > ADC_MAX_PRESSURE) {
    //    GCS_LOG_DEBUG("LoadCell:: ADC value out of range.");
    //    return PlotConfig::INVALID_VALUE;
    //}

    const float voltage = (adcValue / PRESSURE_SENSOR_ADC_RANGE_BIT) * PRESSURE_SENSOR_VOLTAGE_RANGE_V;
    return (float) ((PRESSURE_SENSOR_ARRAY_SLOPE[sensorIndex] * voltage) + PRESSURE_SENSOR_ARRAY_CONST[sensorIndex]);
}*/

#pragma once

namespace GSDataCenterConfig {
constexpr size_t THERMISTOR_AMOUNT_PER_BOARD = 8;
constexpr size_t PRESSURE_SENSOR_AMOUNT_PER_BOARD = 3;
constexpr size_t LOAD_CELL_AMOUNT = 2;
constexpr size_t THERMOCOUPLE_AMOUNT = 4; // FCU: 4 x MAX31856 (common-protocol THERMOCOUPLE_COUNT)
constexpr size_t ADC_CHANNEL_AMOUNT = 8;  // common-protocol ADC_CHANNEL_COUNT (streaming ADC channels)
constexpr size_t VALVE_AMOUNT = 4;

constexpr float TANK_LOAD_CELL_GAIN = 64.0f;
constexpr float TANK_LOAD_CELL_V_EXPERIMENTAL = 5.0f;

constexpr float THRUST_LOAD_CELL_GAIN = 128.0f;
constexpr float THRUST_LOAD_CELL_V_EXPERIMENTAL = 5.0f;

constexpr float TANK_PRESSURE_TRANSDUCER_GAIN = 1.0f; // TODO Figure out
constexpr float TANK_PRESSURE_TRANSDUCER_V_ZERO = 0.0f; // TODO Figure out

constexpr float CHAMBER_PRESSURE_TRANSDUCER_GAIN = 1.0f; // TODO Figure out
constexpr float CHAMBER_PRESSURE_TRANSDUCER_V_ZERO = 0.0f; // TODO Figure out

constexpr float NOS_PRESSURE_TRANSDUCER_GAIN = 1.0f; // TODO Figure out
constexpr float NOS_PRESSURE_TRANSDUCER_V_ZERO = 0.0f; // TODO Figure out
} // namespace GSDataCenterConfig

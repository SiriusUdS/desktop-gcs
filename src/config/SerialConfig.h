#pragma once

namespace SerialConfig {
constexpr size_t MAX_PACKET_SIZE = 64;
constexpr size_t PACKET_CIRCULAR_BUFFER_SIZE = 10'000;
constexpr size_t SERIAL_TASK_LOOPS_PER_SECOND = 20;

// TODO: Move constants below to another config file?
constexpr size_t THERMISTOR_ADC_VALUES_INDEX_OFFSET = 0;
constexpr size_t PRESSURE_SENSOR_ADC_VALUES_INDEX_OFFSET = 10;
constexpr size_t LOAD_CELL_ADC_VALUES_INDEX_OFFSET = 14;

constexpr size_t NOS_VALVE_STATUS_INDEX = 0;
constexpr size_t IPA_VALVE_STATUS_INDEX = 1;

constexpr size_t FILL_VALVE_STATUS_INDEX = 0;
constexpr size_t DUMP_VALVE_STATUS_INDEX = 1;
} // namespace SerialConfig

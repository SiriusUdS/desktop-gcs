#pragma once

#include <cstddef>
#include <cstdint>

// ============================================================================
// Sensor channel map — the single place to (re)map ADC channels / thermocouple
// indices to physical sensors and pick each converter's calibration.
//
// Boards: fcu=false -> ECU (engine/motor) board ADC; fcu=true -> FCU (filling
// station) board ADC. Raw averaged channel values come from
// GSDataCenter::motorAdcAverager (ECU) / fillingStationAdcAverager (FCU).
//
// NOTE: the streaming ADC is an 8-channel ADS131M08 reporting signed 24-bit
// counts, while the converters (PressureTransducer / TemperatureSensor /
// LoadCell) were written for a 12-bit ADC. kAdcScale is applied to every raw
// channel before conversion — calibrate it on hardware.
// ============================================================================
namespace SensorChannelMap {

inline constexpr float kAdcScale = 1.0f; // TODO(calibrate): ADS131M08 24-bit -> converter 12-bit.

// Pressure transducer: board + ADC channel, and the PressureTransducer calibration
// index (0=Fill IPA, 1=Fill NOS, 2=Eng Chamber, 3=Eng Tank).
struct PressureSignal {
    const char* label;
    bool fcu;
    int adcChannel;
    uint16_t sensorIndex;
};
inline constexpr PressureSignal kPressure[] = {
    {"Chamber", /*fcu=*/false, /*ch=*/4, /*calib=*/2}, // ECU ch4 -> Eng Chamber
    {"Tank",    /*fcu=*/false, /*ch=*/3, /*calib=*/3}, // ECU ch3 -> Eng Tank
};

// Thermistor: board + ADC channel (converted via TemperatureSensor).
struct TempSignal {
    const char* label;
    bool fcu;
    int adcChannel;
};
inline constexpr TempSignal kThermistor[] = {
    {"Tank", /*fcu=*/false, /*ch=*/2}, // ECU ch2
};

// Load cell: board + ADC channel, the LoadCell calibration index (0=tank, 1=chamber/motor),
// and the display unit + scale applied to the converter's pounds output (lb -> displayScale).
struct LoadSignal {
    const char* label;
    bool fcu;
    int adcChannel;
    std::size_t loadCellIndex;
    float displayScale; // multiply the converter's lb value to get the displayed unit
};
inline constexpr float kLbToNewton = 4.4482216153f;
inline constexpr float kLbToKg     = 0.45359237f;
inline constexpr LoadSignal kLoad[] = {
    {"Thrust (N)",     /*fcu=*/true, /*ch=*/0, /*calib=*/1, kLbToNewton}, // FCU ch0 -> motor/chamber LC, shown in N
    {"Tank Mass (kg)", /*fcu=*/true, /*ch=*/2, /*calib=*/0, kLbToKg},     // FCU ch2 -> tank LC, shown in kg
};

// Thermocouples (FCU MAX31856) shown on the dashboard: the index into
// GSDataCenter::fillingStationThermocouple_C[] and the display label.
struct ThermocoupleSignal {
    const char* label;
    unsigned index;
};
inline constexpr ThermocoupleSignal kThermocouple[] = {
    {"Top",    0}, // TC1
    {"Throat", 1}, // TC2
};

inline constexpr std::size_t kPressureCount     = sizeof(kPressure) / sizeof(kPressure[0]);
inline constexpr std::size_t kThermistorCount   = sizeof(kThermistor) / sizeof(kThermistor[0]);
inline constexpr std::size_t kLoadCount         = sizeof(kLoad) / sizeof(kLoad[0]);
inline constexpr std::size_t kThermocoupleCount = sizeof(kThermocouple) / sizeof(kThermocouple[0]);

// Signals that feed derived values / the calibration UI.
inline constexpr int kTankThermistor = 0; // index into kThermistor (NOS phase: tank temp)
inline constexpr int kTankPressure   = 1; // index into kPressure   (NOS phase: tank pressure)
inline constexpr int kTankLoad       = 1; // index into kLoad       (tank load cell: Empty/IPA/NOS cal)

} // namespace SensorChannelMap

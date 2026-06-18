#pragma once

#include "GSDataCenterConfig.h"
#include "SensorPlotData.h"

#include <array>
#include "units.h"

// clang-format off
struct ThermistorMotorPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius)
    };

    SensorPlotData& t1() { return data[0]; }
    SensorPlotData& t2() { return data[1]; }
    SensorPlotData& tank() { return data[2]; }
    SensorPlotData& t4() { return data[3]; }
    SensorPlotData& t5() { return data[4]; }
    SensorPlotData& t6() { return data[5]; }
    SensorPlotData& t7() { return data[6]; }
    SensorPlotData& t8() { return data[7]; }
};

struct PressureSensorMotorPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi)
    };

    SensorPlotData& tank() { return data[0]; }
    SensorPlotData& chamber() { return data[1]; }
    SensorPlotData& NOS() { return data[2]; }
};

struct ThermistorFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius)
    };

    SensorPlotData& tank_top() { return data[0]; }
    SensorPlotData& tank_bottom() { return data[1]; }
    SensorPlotData& t3() { return data[2]; }
    SensorPlotData& t4() { return data[3]; }
    SensorPlotData& t5() { return data[4]; }
    SensorPlotData& t6() { return data[5]; }
    SensorPlotData& t7() { return data[6]; }
    SensorPlotData& t8() { return data[7]; }
};

struct PressureSensorFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::PressureUnit::Psi)
    };

    SensorPlotData& p1() { return data[0]; }
    SensorPlotData& p2() { return data[1]; }
    SensorPlotData& p3() { return data[2]; }
};

struct LoadCellFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::LOAD_CELL_AMOUNT> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::ForceUnit::PoundsForce),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::WeightUnit::Pounds)
    };

    SensorPlotData& motor() { return data[0]; }
    SensorPlotData& tank() { return data[1]; }
};

struct ThermocoupleFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::THERMOCOUPLE_AMOUNT> data{
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
        SensorPlotData(Units::TimeUnit::Milliseconds, Units::TemperatureUnit::Celcius),
    };

    SensorPlotData& tc1() { return data[0]; }
    SensorPlotData& tc2() { return data[1]; }
    SensorPlotData& tc3() { return data[2]; }
    SensorPlotData& tc4() { return data[3]; }
};
// clang-format on

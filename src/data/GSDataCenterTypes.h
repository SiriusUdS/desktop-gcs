#pragma once

#include "GSDataCenterConfig.h"
#include "SensorPlotData.h"

#include <array>

// clang-format off
struct ThermistorMotorPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> data;

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
    std::array<SensorPlotData, GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD> data;

    SensorPlotData& tank() { return data[0]; }
    SensorPlotData& p2() { return data[1]; }
};

struct ThermistorFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> data;

    SensorPlotData& t1() { return data[0]; }
    SensorPlotData& t2() { return data[1]; }
    SensorPlotData& t3() { return data[2]; }
    SensorPlotData& t4() { return data[3]; }
    SensorPlotData& t5() { return data[4]; }
    SensorPlotData& t6() { return data[5]; }
    SensorPlotData& t7() { return data[6]; }
    SensorPlotData& t8() { return data[7]; }
};

struct PressureSensorFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD> data;

    SensorPlotData& p1() { return data[0]; }
    SensorPlotData& p2() { return data[1]; }
};

struct LoadCellFillingStationPlotData {
    std::array<SensorPlotData, GSDataCenterConfig::LOAD_CELL_AMOUNT> data;

    SensorPlotData& motor() { return data[0]; }
    SensorPlotData& tank() { return data[1]; }
};
// clang-format on

#pragma once

#include "CSVLogger.h"
#include "GSDataCenterConfig.h"

#include "devices/valve/valve_info.hpp"

namespace PacketCSVLogging {
// --- Telemetry (ADC sensor) logs -------------------------------------------
// Fed from the SystemState decode once the new AdcInfo channel map and the
// int32 -> engineering-unit conversions are wired (see PacketProcessing TODO).
void logEngineTelemetryPacket(float timestamp,
                              uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                              float thermistorValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                              uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                              float pressureSensorValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD]);
void logFillingStationTelemetryPacket(float timestamp,
                                      uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                      float thermistorValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                      uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                      float pressureSensorValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                      uint16_t loadCellAdcValues[GSDataCenterConfig::LOAD_CELL_AMOUNT],
                                      float loadCellValues[GSDataCenterConfig::LOAD_CELL_AMOUNT]);

// --- Status logs -----------------------------------------------------------
// Board state, per-valve telemetry (common-protocol ValveInfo) and storage
// error, fed from the SystemState decode.
void logEngineStatus(float timestamp, uint8_t boardState, const ValveInfo& nosValve, const ValveInfo& ipaValve, uint16_t storageError);
void logFillingStationStatus(float timestamp, uint8_t boardState, const ValveInfo& fillValve, const ValveInfo& dumpValve, uint16_t storageError);
} // namespace PacketCSVLogging

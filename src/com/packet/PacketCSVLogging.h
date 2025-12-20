#pragma once

#include "CSVLogger.h"
#include "GSDataCenterConfig.h"
#include "Telecommunication/TelemetryPacket.h"

namespace PacketCSVLogging {
void init();
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
void logGSControlPacket(const GSControlStatusPacket* packet);
void logEngineStatusPacket(const EngineStatusPacket* packet);
void logFillingStationStatusPacket(const FillingStationStatusPacket* packet);
} // namespace PacketCSVLogging

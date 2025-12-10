#pragma once

#include "CSVLogger.h"
#include "GSDataCenter.h"
#include "Telecommunication/TelemetryPacket.h"

namespace PacketCSVLogging {
void init();
void logEngineTelemetryPacket(float timestamp,
                              uint16_t thermistorAdcValues[GSDataCenter::THERMISTOR_AMOUNT_PER_BOARD],
                              float thermistorValues[GSDataCenter::THERMISTOR_AMOUNT_PER_BOARD],
                              uint16_t pressureSensorAdcValues[GSDataCenter::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                              float pressureSensorValues[GSDataCenter::PRESSURE_SENSOR_AMOUNT_PER_BOARD]);
void logFillingStationTelemetryPacket(float timestamp,
                                      uint16_t thermistorAdcValues[GSDataCenter::THERMISTOR_AMOUNT_PER_BOARD],
                                      float thermistorValues[GSDataCenter::THERMISTOR_AMOUNT_PER_BOARD],
                                      uint16_t pressureSensorAdcValues[GSDataCenter::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                      float pressureSensorValues[GSDataCenter::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                      uint16_t loadCellAdcValues[GSDataCenter::LOAD_CELL_AMOUNT],
                                      float loadCellValues[GSDataCenter::LOAD_CELL_AMOUNT]);
void logGSControlPacket(const GSControlStatusPacket* packet);
void logEngineStatusPacket(const EngineStatusPacket* packet);
void logFillingStationStatusPacket(const FillingStationStatusPacket* packet);
} // namespace PacketCSVLogging

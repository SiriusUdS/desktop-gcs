#pragma once

#include "AdcChannelAverager.h"
#include "GSDataCenterTypes.h"
#include "SwitchData.h"
#include "ValveData.h"

#include "telemetry/ecu_extended_system_state.hpp"
#include "telemetry/ecu_system_state.hpp"
#include "telemetry/fcu_extended_system_state.hpp"
#include "telemetry/fcu_system_state.hpp"
#include "telemetry/gs_system_state.hpp"

#include <atomic>
#include <vector>

namespace GSDataCenter {
extern ThermistorMotorPlotData Thermistor_Motor_PlotData;
extern PressureSensorMotorPlotData PressureSensor_Motor_PlotData;
extern ThermistorFillingStationPlotData Thermistor_FillingStation_PlotData;
extern PressureSensorFillingStationPlotData PressureSensor_FillingStation_PlotData;
extern LoadCellFillingStationPlotData LoadCell_FillingStation_PlotData;
extern ThermocoupleFillingStationPlotData Thermocouple_FillingStation_PlotData;

extern PlotData NOSTankMass_PlotData;
extern PlotData TankMass_PlotData;
extern PlotData TankGasLeft_perc_PlotData;

extern SwitchData AllowDumpSwitchData;
extern SwitchData AllowFillSwitchData;
extern SwitchData ArmIgniterSwitchData;
extern SwitchData ArmServoSwitchData;
extern SwitchData EmergencyStopButtonData;
extern SwitchData FireIgniterButtonData;
extern SwitchData UnsafeKeySwitchData;
extern SwitchData ValveStartButtonData;

extern ValveData nosValveData;
extern ValveData ipaValveData;
extern ValveData fillValveData;
extern ValveData dumpValveData;
extern std::vector<ValveData*> valveDataVec;

extern std::atomic<uint8_t> motorBoardState;
extern std::atomic<uint8_t> fillingStationBoardState;
extern std::atomic<uint8_t> gsControlBoardState;

extern std::atomic<uint16_t> motorBoardStorageErrorStatus;
extern std::atomic<uint16_t> fillingStationBoardStorageErrorStatus;

extern std::atomic<uint32_t> motorBoardTimestamp_ms;
extern std::atomic<uint32_t> fillingStationBoardTimestamp_ms;

extern std::atomic<uint32_t> motorBoardControlFlags;
extern std::atomic<uint32_t> fillingStationBoardControlFlags;

extern std::atomic<float> fillingStationThermocouple_C[GSDataCenterConfig::THERMOCOUPLE_AMOUNT];
extern std::atomic<uint8_t> fillingStationThermocoupleState[GSDataCenterConfig::THERMOCOUPLE_AMOUNT];

extern AdcChannelAverager motorAdcAverager;
extern AdcChannelAverager fillingStationAdcAverager;

extern std::atomic<uint32_t> pongReceivedCount;
extern std::atomic<uint8_t> lastPongSenderId;

extern std::atomic<uint32_t> igniteTimestamp_ms;
extern std::atomic<uint32_t> launchTimestamp_ms;

extern std::atomic<uint32_t> lastReceivedCommandCodeMotorBoard;
extern std::atomic<uint32_t> lastReceivedCommandCodeFillingStationBoard;
extern std::atomic<uint32_t> lastBoardSentCommandCode;

extern std::atomic<uint32_t> timeSinceLastCommandMotorBoard_ms;
extern std::atomic<uint32_t> timeSinceLastCommandFillingStationBoard_ms;
extern std::atomic<uint32_t> lastReceivedGSCommandTimestamp_ms;
extern std::atomic<uint32_t> lastSentCommandTimestamp_ms;

// Cumulative decoded-record counts per telemetry type (the UI derives Hz from deltas).
extern std::atomic<uint64_t> ecuSystemStateCount;
extern std::atomic<uint64_t> fcuSystemStateCount;
extern std::atomic<uint64_t> ecuExtendedSystemStateCount;
extern std::atomic<uint64_t> fcuExtendedSystemStateCount;
extern std::atomic<uint64_t> gsSystemStateCount;

// Most recent fully-decoded telemetry record of each type, kept verbatim so the UI
// can show every field (the curated atomics above hold only the subset used elsewhere).
// The matching *Count above is non-zero once a record has landed here.
extern std::atomic<EcuSystemState> latestEcuSystemState;
extern std::atomic<FcuSystemState> latestFcuSystemState;
extern std::atomic<EcuExtendedSystemState> latestEcuExtendedSystemState;
extern std::atomic<FcuExtendedSystemState> latestFcuExtendedSystemState;
extern std::atomic<GSSystemState> latestGsSystemState;
} // namespace GSDataCenter

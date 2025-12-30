#pragma once

#include "GSDataCenterTypes.h"
#include "SwitchData.h"
#include "ValveData.h"

#include <atomic>
#include <vector>

namespace GSDataCenter {
extern ThermistorMotorPlotData Thermistor_Motor_PlotData;
extern PressureSensorMotorPlotData PressureSensor_Motor_PlotData;
extern ThermistorFillingStationPlotData Thermistor_FillingStation_PlotData;
extern PressureSensorFillingStationPlotData PressureSensor_FillingStation_PlotData;
extern LoadCellFillingStationPlotData LoadCell_FillingStation_PlotData;

extern PlotData NOSTankMass_PlotData;
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

extern std::atomic<uint32_t> igniteTimestamp_ms;
extern std::atomic<uint32_t> launchTimestamp_ms;

extern std::atomic<uint32_t> lastReceivedCommandCodeMotorBoard;
extern std::atomic<uint32_t> lastReceivedCommandCodeFillingStationBoard;
extern std::atomic<uint32_t> lastBoardSentCommandCode;

extern std::atomic<uint32_t> timeSinceLastCommandMotorBoard_ms;
extern std::atomic<uint32_t> timeSinceLastCommandFillingStationBoard_ms;
extern std::atomic<uint32_t> lastReceivedGSCommandTimestamp_ms;
extern std::atomic<uint32_t> lastSentCommandTimestamp_ms;
} // namespace GSDataCenter

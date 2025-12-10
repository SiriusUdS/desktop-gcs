#pragma once

#include <atomic>
#include <vector>

class PlotData;
class SensorPlotData;
struct SwitchData;
struct ValveStateData;
struct ValveData;

namespace GSDataCenter {
constexpr size_t THERMISTOR_AMOUNT_PER_BOARD = 8;
constexpr size_t PRESSURE_SENSOR_AMOUNT_PER_BOARD = 2;
constexpr size_t LOAD_CELL_AMOUNT = 2;
constexpr size_t VALVE_AMOUNT = 4;

extern SensorPlotData Thermistor_Motor_PlotData[THERMISTOR_AMOUNT_PER_BOARD];
extern SensorPlotData PressureSensor_Motor_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD];
extern SensorPlotData Thermistor_FillingStation_PlotData[THERMISTOR_AMOUNT_PER_BOARD];
extern SensorPlotData PressureSensor_FillingStation_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD];
extern SensorPlotData LoadCell_FillingStation_PlotData[LOAD_CELL_AMOUNT];

extern PlotData NOSTankMass_PlotData;

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

// TODO: These are temporary
extern std::atomic<float> tankTemperature_C;
extern std::atomic<float> tankPressure_psi;
extern std::atomic<float> tankLoadCell_lb;
} // namespace GSDataCenter

#ifndef GSDATACENTER_H
#define GSDATACENTER_H

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

extern uint8_t motorBoardState;
extern uint8_t fillingStationBoardState;
extern uint8_t gsControlBoardState;

extern uint16_t motorBoardStorageErrorStatus;
extern uint16_t fillingStationBoardStorageErrorStatus;

extern uint32_t igniteTimestamp_ms;
extern uint32_t launchTimestamp_ms;

extern uint32_t lastReceivedCommandCodeMotorBoard;
extern uint32_t lastReceivedCommandCodeFillingStationBoard;
extern uint32_t lastBoardSentCommandCode;

extern uint32_t timeSinceLastCommandMotorBoard_ms;
extern uint32_t timeSinceLastCommandFillingStationBoard_ms;
extern uint32_t lastReceivedGSCommandTimestamp_ms;
extern uint32_t lastSentCommandTimestamp_ms;

// TODO: These are temporary
extern float tankTemperature_C;
extern float tankPressure_psi;
extern float tankLoadCell_lb;
} // namespace GSDataCenter

#endif // GSDATACENTER_H

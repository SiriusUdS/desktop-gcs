#include "GSDataCenter.h"

#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ValveData.h"

namespace GSDataCenter {
ThermistorMotorPlotData Thermistor_Motor_PlotData;
PressureSensorMotorPlotData PressureSensor_Motor_PlotData;
ThermistorFillingStationPlotData Thermistor_FillingStation_PlotData;
PressureSensorFillingStationPlotData PressureSensor_FillingStation_PlotData;
LoadCellFillingStationPlotData LoadCell_FillingStation_PlotData;

PlotData NOSTankMass_PlotData;

SwitchData AllowDumpSwitchData;
SwitchData AllowFillSwitchData;
SwitchData ArmIgniterSwitchData;
SwitchData ArmServoSwitchData;
SwitchData EmergencyStopButtonData;
SwitchData FireIgniterButtonData;
SwitchData UnsafeKeySwitchData;
SwitchData ValveStartButtonData;

ValveData nosValveData;
ValveData ipaValveData;
ValveData fillValveData;
ValveData dumpValveData;
std::vector<ValveData*> valveDataVec({&fillValveData, &dumpValveData});

std::atomic<uint8_t> motorBoardState{};
std::atomic<uint8_t> fillingStationBoardState{};
std::atomic<uint8_t> gsControlBoardState{};

std::atomic<uint16_t> motorBoardStorageErrorStatus{};
std::atomic<uint16_t> fillingStationBoardStorageErrorStatus{};

std::atomic<uint32_t> igniteTimestamp_ms{};
std::atomic<uint32_t> launchTimestamp_ms{};

std::atomic<uint32_t> lastReceivedCommandCodeMotorBoard{};
std::atomic<uint32_t> lastReceivedCommandCodeFillingStationBoard{};
std::atomic<uint32_t> lastBoardSentCommandCode;

std::atomic<uint32_t> timeSinceLastCommandMotorBoard_ms{};
std::atomic<uint32_t> timeSinceLastCommandFillingStationBoard_ms{};
std::atomic<uint32_t> lastReceivedGSCommandTimestamp_ms{};
std::atomic<uint32_t> lastSentCommandTimestamp_ms{};
} // namespace GSDataCenter

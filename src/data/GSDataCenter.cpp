#include "GSDataCenter.h"

#include "SwitchData.h"
#include "ValveData.h"
#include "units.h"

namespace GSDataCenter {
ThermistorMotorPlotData Thermistor_Motor_PlotData;
PressureSensorMotorPlotData PressureSensor_Motor_PlotData;
ThermistorFillingStationPlotData Thermistor_FillingStation_PlotData;
PressureSensorFillingStationPlotData PressureSensor_FillingStation_PlotData;
LoadCellFillingStationPlotData LoadCell_FillingStation_PlotData;
ThermocoupleFillingStationPlotData Thermocouple_FillingStation_PlotData;

PlotData NOSTankMass_PlotData(Units::TimeUnit::Seconds, Units::WeightUnit::Pounds); // TODO What unit is this provided in
PlotData TankMass_PlotData(Units::TimeUnit::Seconds, Units::WeightUnit::Pounds); // TODO What unit is this provided in
PlotData TankGasLeft_perc_PlotData(Units::TimeUnit::Seconds, Units::QuantityUnit::Percentage); // TODO What unit is this provided in

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

std::atomic<uint32_t> motorBoardTimestamp_ms{};
std::atomic<uint32_t> fillingStationBoardTimestamp_ms{};

std::atomic<uint32_t> motorBoardControlFlags{};
std::atomic<uint32_t> fillingStationBoardControlFlags{};

std::atomic<float> fillingStationThermocouple_C[GSDataCenterConfig::THERMOCOUPLE_AMOUNT]{};
std::atomic<uint8_t> fillingStationThermocoupleState[GSDataCenterConfig::THERMOCOUPLE_AMOUNT]{};

AdcChannelAverager motorAdcAverager;
AdcChannelAverager fillingStationAdcAverager;

std::atomic<uint32_t> pongReceivedCount{};
std::atomic<uint8_t> lastPongSenderId{};

std::atomic<uint32_t> igniteTimestamp_ms{};
std::atomic<uint32_t> launchTimestamp_ms{};

std::atomic<uint32_t> lastReceivedCommandCodeMotorBoard{};
std::atomic<uint32_t> lastReceivedCommandCodeFillingStationBoard{};
std::atomic<uint32_t> lastBoardSentCommandCode;

std::atomic<uint32_t> timeSinceLastCommandMotorBoard_ms{};
std::atomic<uint32_t> timeSinceLastCommandFillingStationBoard_ms{};
std::atomic<uint32_t> lastReceivedGSCommandTimestamp_ms{};
std::atomic<uint32_t> lastSentCommandTimestamp_ms{};

std::atomic<uint64_t> ecuSystemStateCount{};
std::atomic<uint64_t> fcuSystemStateCount{};
std::atomic<uint64_t> ecuExtendedSystemStateCount{};
std::atomic<uint64_t> fcuExtendedSystemStateCount{};
std::atomic<uint64_t> gsSystemStateCount{};

std::atomic<EcuSystemState> latestEcuSystemState{};
std::atomic<FcuSystemState> latestFcuSystemState{};
std::atomic<EcuExtendedSystemState> latestEcuExtendedSystemState{};
std::atomic<FcuExtendedSystemState> latestFcuExtendedSystemState{};
std::atomic<GSSystemState> latestGsSystemState{};
} // namespace GSDataCenter

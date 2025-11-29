#include "GSDataCenter.h"

#include "Engine/EngineSensors.h"
#include "FillingStation/FillingStationSensors.h"
#include "PlotColors.h"
#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ValveData.h"

namespace GSDataCenter {
SensorPlotData Thermistor_Motor_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", PlotColors::BLUE},
                                                                         {"Thermistor 2", PlotColors::RED},
                                                                         {"Tank Thermistor", PlotColors::GREEN},
                                                                         {"Thermistor 4", PlotColors::YELLOW},
                                                                         {"Thermistor 5", PlotColors::ORANGE},
                                                                         {"Thermistor 6", PlotColors::CYAN},
                                                                         {"Thermistor 7", PlotColors::PURPLE},
                                                                         {"Thermistor 8", PlotColors::GRAY}};
SensorPlotData PressureSensor_Motor_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Motor)", PlotColors::BLUE},
                                                                                  {"Pressure Sensor 2 (Motor)", PlotColors::RED}};
SensorPlotData Thermistor_FillingStation_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", PlotColors::BLUE},
                                                                                  {"Thermistor 2", PlotColors::RED},
                                                                                  {"Thermistor 3", PlotColors::GREEN},
                                                                                  {"Thermistor 4", PlotColors::YELLOW},
                                                                                  {"Thermistor 5", PlotColors::ORANGE},
                                                                                  {"Thermistor 6", PlotColors::CYAN},
                                                                                  {"Thermistor 7", PlotColors::PURPLE},
                                                                                  {"Thermistor 8", PlotColors::GRAY}};
SensorPlotData PressureSensor_FillingStation_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Fill)", PlotColors::BLUE},
                                                                                           {"Pressure Sensor 2 (Fill)", PlotColors::RED}};
SensorPlotData LoadCell_FillingStation_PlotData[LOAD_CELL_AMOUNT] = {{"Motor Load Cell", PlotColors::BLUE}, {"Tank Load Cell", PlotColors::RED}};

PlotData NOSTankMass_PlotData{"NOS Tank Mass (Computed)", PlotColors::BLUE};

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

std::atomic<uint16_t> motorBoardStorageErrorStatus;
std::atomic<uint16_t> fillingStationBoardStorageErrorStatus;

std::atomic<uint32_t> igniteTimestamp_ms{};
std::atomic<uint32_t> launchTimestamp_ms{};

std::atomic<uint32_t> lastReceivedCommandCodeMotorBoard{};
std::atomic<uint32_t> lastReceivedCommandCodeFillingStationBoard{};
std::atomic<uint32_t> lastBoardSentCommandCode;

std::atomic<uint32_t> timeSinceLastCommandMotorBoard_ms{};
std::atomic<uint32_t> timeSinceLastCommandFillingStationBoard_ms{};
std::atomic<uint32_t> lastReceivedGSCommandTimestamp_ms{};
std::atomic<uint32_t> lastSentCommandTimestamp_ms{};

std::atomic<float> tankTemperature_C{};
std::atomic<float> tankPressure_psi{};
std::atomic<float> tankLoadCell_lb{};
} // namespace GSDataCenter

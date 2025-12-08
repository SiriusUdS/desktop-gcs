#include "GSDataCenter.h"

#include "Engine/EngineSensors.h"
#include "FillingStation/FillingStationSensors.h"
#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ThemedColors.h"
#include "ValveData.h"

namespace GSDataCenter {
SensorPlotData Thermistor_Motor_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", ThemedColors::PlotLine::blue},
                                                                         {"Thermistor 2", ThemedColors::PlotLine::red},
                                                                         {"Tank Thermistor", ThemedColors::PlotLine::green},
                                                                         {"Thermistor 4", ThemedColors::PlotLine::yellow},
                                                                         {"Thermistor 5", ThemedColors::PlotLine::orange},
                                                                         {"Thermistor 6", ThemedColors::PlotLine::cyan},
                                                                         {"Thermistor 7", ThemedColors::PlotLine::purple},
                                                                         {"Thermistor 8", ThemedColors::PlotLine::gray}};
SensorPlotData PressureSensor_Motor_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Motor)", ThemedColors::PlotLine::blue},
                                                                                  {"Pressure Sensor 2 (Motor)", ThemedColors::PlotLine::red}};
SensorPlotData Thermistor_FillingStation_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", ThemedColors::PlotLine::blue},
                                                                                  {"Thermistor 2", ThemedColors::PlotLine::red},
                                                                                  {"Thermistor 3", ThemedColors::PlotLine::green},
                                                                                  {"Thermistor 4", ThemedColors::PlotLine::yellow},
                                                                                  {"Thermistor 5", ThemedColors::PlotLine::orange},
                                                                                  {"Thermistor 6", ThemedColors::PlotLine::cyan},
                                                                                  {"Thermistor 7", ThemedColors::PlotLine::purple},
                                                                                  {"Thermistor 8", ThemedColors::PlotLine::gray}};
SensorPlotData PressureSensor_FillingStation_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Fill)", ThemedColors::PlotLine::blue},
                                                                                           {"Pressure Sensor 2 (Fill)", ThemedColors::PlotLine::red}};
SensorPlotData LoadCell_FillingStation_PlotData[LOAD_CELL_AMOUNT] = {{"Motor Load Cell", ThemedColors::PlotLine::blue},
                                                                     {"Tank Load Cell", ThemedColors::PlotLine::red}};

PlotData NOSTankMass_PlotData{"NOS Tank Mass (Computed)", ThemedColors::PlotLine::blue};

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

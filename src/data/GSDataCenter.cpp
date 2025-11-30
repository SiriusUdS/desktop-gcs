#include "GSDataCenter.h"

#include "Engine/EngineSensors.h"
#include "FillingStation/FillingStationSensors.h"
#include "SensorPlotData.h"
#include "SwitchData.h"
#include "ThemedColors.h"
#include "ValveData.h"

namespace GSDataCenter {
SensorPlotData Thermistor_Motor_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", ThemedColors::BLUE_PLOT_LINE},
                                                                         {"Thermistor 2", ThemedColors::RED_PLOT_LINE},
                                                                         {"Tank Thermistor", ThemedColors::GREEN_PLOT_LINE},
                                                                         {"Thermistor 4", ThemedColors::YELLOW_PLOT_LINE},
                                                                         {"Thermistor 5", ThemedColors::ORANGE_PLOT_LINE},
                                                                         {"Thermistor 6", ThemedColors::CYAN_PLOT_LINE},
                                                                         {"Thermistor 7", ThemedColors::PURPLE_PLOT_LINE},
                                                                         {"Thermistor 8", ThemedColors::GRAY_PLOT_LINE}};
SensorPlotData PressureSensor_Motor_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Motor)", ThemedColors::BLUE_PLOT_LINE},
                                                                                  {"Pressure Sensor 2 (Motor)", ThemedColors::RED_PLOT_LINE}};
SensorPlotData Thermistor_FillingStation_PlotData[THERMISTOR_AMOUNT_PER_BOARD] = {{"Thermistor 1", ThemedColors::BLUE_PLOT_LINE},
                                                                                  {"Thermistor 2", ThemedColors::RED_PLOT_LINE},
                                                                                  {"Thermistor 3", ThemedColors::GREEN_PLOT_LINE},
                                                                                  {"Thermistor 4", ThemedColors::YELLOW_PLOT_LINE},
                                                                                  {"Thermistor 5", ThemedColors::ORANGE_PLOT_LINE},
                                                                                  {"Thermistor 6", ThemedColors::CYAN_PLOT_LINE},
                                                                                  {"Thermistor 7", ThemedColors::PURPLE_PLOT_LINE},
                                                                                  {"Thermistor 8", ThemedColors::GRAY_PLOT_LINE}};
SensorPlotData PressureSensor_FillingStation_PlotData[PRESSURE_SENSOR_AMOUNT_PER_BOARD] = {{"Pressure Sensor 1 (Fill)", ThemedColors::BLUE_PLOT_LINE},
                                                                                           {"Pressure Sensor 2 (Fill)", ThemedColors::RED_PLOT_LINE}};
SensorPlotData LoadCell_FillingStation_PlotData[LOAD_CELL_AMOUNT] = {{"Motor Load Cell", ThemedColors::BLUE_PLOT_LINE},
                                                                     {"Tank Load Cell", ThemedColors::RED_PLOT_LINE}};

PlotData NOSTankMass_PlotData{"NOS Tank Mass (Computed)", ThemedColors::BLUE_PLOT_LINE};

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

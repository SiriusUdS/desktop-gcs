#include "PacketCSVLogging.h"

#include "Logging.h"

#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

namespace PacketCSVLogging {
void createDataLogDir();
std::string getCurrentDateTimeForDir();
void initEngineTelemetryLogger();
void initFillingStationTelemetryLogger();
void initEngineStatusLogger();
void initFillingStationStatusLogger();

std::string dataLogDir;

CSVLogger engineTelemetryLogger;
CSVLogger fillingStationTelemetryLogger;
CSVLogger engineStatusLogger;
CSVLogger fillingStationStatusLogger;

bool triedCreatingDataLogDir{};
bool dataLogDirExists{};

bool engineTelemetryHasInit{};
bool fillingStationTelemetryHasInit{};
bool engineStatusHasInit{};
bool fillingStationStatusHasInit{};
} // namespace PacketCSVLogging

void PacketCSVLogging::logEngineTelemetryPacket(float timestamp,
                                                uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                                float thermistorValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                                uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                                float pressureSensorValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD]) {
    if (!engineTelemetryHasInit) {
        initEngineTelemetryLogger();
        engineTelemetryHasInit = true;
    }

    if (!dataLogDirExists) {
        return;
    }

    if (!engineTelemetryLogger.fileIsOpen()) {
        GCS_APP_LOG_WARN("PacketCSVLogging: Can't log engine telemetry packet, log file isn't open.");
        return;
    }

    engineTelemetryLogger.setValue(0, timestamp);

    size_t columnOffset = 1;
    for (size_t i = 0; i < GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD; i++) {
        const size_t offset = columnOffset + i * 2;
        engineTelemetryLogger.setValue(offset, thermistorAdcValues[i]);
        engineTelemetryLogger.setValue(offset + 1, thermistorValues[i]);
    }

    columnOffset += GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD * 2;
    for (size_t i = 0; i < GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD; i++) {
        const size_t offset = columnOffset + i * 2;
        engineTelemetryLogger.setValue(offset, pressureSensorAdcValues[i]);
        engineTelemetryLogger.setValue(offset + 1, pressureSensorValues[i]);
    }

    engineTelemetryLogger.log();
}

void PacketCSVLogging::logFillingStationTelemetryPacket(float timestamp,
                                                        uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                                        float thermistorValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD],
                                                        uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                                        float pressureSensorValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                                        uint16_t loadCellAdcValues[GSDataCenterConfig::LOAD_CELL_AMOUNT],
                                                        float loadCellValues[GSDataCenterConfig::LOAD_CELL_AMOUNT]) {
    if (!fillingStationTelemetryHasInit) {
        initFillingStationTelemetryLogger();
        fillingStationTelemetryHasInit = true;
    }

    if (!dataLogDirExists) {
        return;
    }

    if (!fillingStationTelemetryLogger.fileIsOpen()) {
        GCS_APP_LOG_WARN("PacketCSVLogging: Can't log filling station telemetry packet, log file isn't open.");
        return;
    }

    fillingStationTelemetryLogger.setValue(0, timestamp);

    size_t columnOffset = 1;
    for (size_t i = 0; i < GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD; i++) {
        const size_t offset = columnOffset + i * 2;
        fillingStationTelemetryLogger.setValue(offset, thermistorAdcValues[i]);
        fillingStationTelemetryLogger.setValue(offset + 1, thermistorValues[i]);
    }

    columnOffset += GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD * 2;
    for (size_t i = 0; i < GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD; i++) {
        const size_t offset = columnOffset + i * 2;
        fillingStationTelemetryLogger.setValue(offset, pressureSensorAdcValues[i]);
        fillingStationTelemetryLogger.setValue(offset + 1, pressureSensorValues[i]);
    }

    columnOffset += GSDataCenterConfig::LOAD_CELL_AMOUNT * 2;
    for (size_t i = 0; i < GSDataCenterConfig::LOAD_CELL_AMOUNT; i++) {
        const size_t offset = columnOffset + i * 2;
        fillingStationTelemetryLogger.setValue(offset, loadCellAdcValues[i]);
        fillingStationTelemetryLogger.setValue(offset + 1, loadCellValues[i]);
    }

    fillingStationTelemetryLogger.log();
}

namespace {
// Append a single valve's telemetry (state, switch flags, commanded %) to a row,
// starting at column `col`; returns the next free column.
size_t logValveInfo(CSVLogger& logger, size_t col, const ValveInfo& valve) {
    logger.setValue(col++, static_cast<float>(static_cast<uint8_t>(valve.state)));
    logger.setValue(col++, static_cast<float>(!valve.status.in_transition)); // idle = not moving
    logger.setValue(col++, static_cast<float>(valve.status.closed_limit_high));
    logger.setValue(col++, static_cast<float>(valve.status.open_limit_high));
    logger.setValue(col++, static_cast<float>(valve.current_set_value));
    return col;
}
} // namespace

void PacketCSVLogging::logEngineStatus(float timestamp,
                                       uint8_t boardState,
                                       const ValveInfo& nosValve,
                                       const ValveInfo& ipaValve,
                                       uint16_t storageError) {
    if (!engineStatusHasInit) {
        initEngineStatusLogger();
        engineStatusHasInit = true;
    }

    if (!dataLogDirExists) {
        return;
    }

    if (!engineStatusLogger.fileIsOpen()) {
        GCS_APP_LOG_WARN("PacketCSVLogging: Can't log engine status, log file isn't open.");
        return;
    }

    size_t col = 0;
    engineStatusLogger.setValue(col++, timestamp);
    engineStatusLogger.setValue(col++, static_cast<float>(boardState));
    col = logValveInfo(engineStatusLogger, col, nosValve);
    col = logValveInfo(engineStatusLogger, col, ipaValve);
    engineStatusLogger.setValue(col++, static_cast<float>(storageError));
    engineStatusLogger.log();
}

void PacketCSVLogging::logFillingStationStatus(float timestamp,
                                               uint8_t boardState,
                                               const ValveInfo& fillValve,
                                               const ValveInfo& dumpValve,
                                               uint16_t storageError) {
    if (!fillingStationStatusHasInit) {
        initFillingStationStatusLogger();
        fillingStationStatusHasInit = true;
    }

    if (!dataLogDirExists) {
        return;
    }

    if (!fillingStationStatusLogger.fileIsOpen()) {
        GCS_APP_LOG_WARN("PacketCSVLogging: Can't log filling station status, log file isn't open.");
        return;
    }

    size_t col = 0;
    fillingStationStatusLogger.setValue(col++, timestamp);
    fillingStationStatusLogger.setValue(col++, static_cast<float>(boardState));
    col = logValveInfo(fillingStationStatusLogger, col, fillValve);
    col = logValveInfo(fillingStationStatusLogger, col, dumpValve);
    fillingStationStatusLogger.setValue(col++, static_cast<float>(storageError));
    fillingStationStatusLogger.log();
}

void PacketCSVLogging::createDataLogDir() {
    dataLogDir = "logs/data/" + getCurrentDateTimeForDir();
    triedCreatingDataLogDir = true;

    try {
        std::filesystem::create_directories(dataLogDir);
        dataLogDirExists = true;
    } catch (const std::filesystem::filesystem_error& e) {
        GCS_APP_LOG_ERROR("PacketCSVLogging: Failed to create directory '{}': {}", dataLogDir, e.what());
    }
}

std::string PacketCSVLogging::getCurrentDateTimeForDir() {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void PacketCSVLogging::initEngineTelemetryLogger() {
    if (!triedCreatingDataLogDir) {
        createDataLogDir();
    }

    engineTelemetryLogger.openFile(dataLogDir + "/EngineTelemetry.log");
    engineTelemetryLogger.addColumn("Timestamp");
    engineTelemetryLogger.addColumn("Thermistor 1 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 1 (C)");
    engineTelemetryLogger.addColumn("Thermistor 2 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 2 (C)");
    engineTelemetryLogger.addColumn("Thermistor 3 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 3 (C)");
    engineTelemetryLogger.addColumn("Thermistor 4 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 4 (C)");
    engineTelemetryLogger.addColumn("Thermistor 5 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 5 (C)");
    engineTelemetryLogger.addColumn("Thermistor 6 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 6 (C)");
    engineTelemetryLogger.addColumn("Thermistor 7 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 7 (C)");
    engineTelemetryLogger.addColumn("Thermistor 8 (ADC)");
    engineTelemetryLogger.addColumn("Thermistor 8 (C)");
    engineTelemetryLogger.addColumn("Pressure Sensor 1 (ADC)");
    engineTelemetryLogger.addColumn("Pressure Sensor 1 (psi)");
    engineTelemetryLogger.addColumn("Pressure Sensor 2 (ADC)");
    engineTelemetryLogger.addColumn("Pressure Sensor 2 (psi)");
}

void PacketCSVLogging::initFillingStationTelemetryLogger() {
    if (!triedCreatingDataLogDir) {
        createDataLogDir();
    }

    fillingStationTelemetryLogger.openFile(dataLogDir + "/FillingStationTelemetry.log");
    fillingStationTelemetryLogger.addColumn("Timestamp");
    fillingStationTelemetryLogger.addColumn("Thermistor 1 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 1 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 2 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 2 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 3 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 3 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 4 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 4 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 5 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 5 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 6 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 6 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 7 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 7 (C)");
    fillingStationTelemetryLogger.addColumn("Thermistor 8 (ADC)");
    fillingStationTelemetryLogger.addColumn("Thermistor 8 (C)");
    fillingStationTelemetryLogger.addColumn("Pressure Sensor 1 (ADC)");
    fillingStationTelemetryLogger.addColumn("Pressure Sensor 1 (psi)");
    fillingStationTelemetryLogger.addColumn("Pressure Sensor 2 (ADC)");
    fillingStationTelemetryLogger.addColumn("Pressure Sensor 2 (psi)");
    fillingStationTelemetryLogger.addColumn("Motor Load Cell (ADC)");
    fillingStationTelemetryLogger.addColumn("Motor Load Cell (lb)");
    fillingStationTelemetryLogger.addColumn("Tank Load Cell (ADC)");
    fillingStationTelemetryLogger.addColumn("Tank Load Cell (lb)");
}

void PacketCSVLogging::initEngineStatusLogger() {
    if (!triedCreatingDataLogDir) {
        createDataLogDir();
    }

    engineStatusLogger.openFile(dataLogDir + "/EngineStatus.log");
    engineStatusLogger.addColumn("Timestamp");
    engineStatusLogger.addColumn("Board State");
    engineStatusLogger.addColumn("NOS Valve State");
    engineStatusLogger.addColumn("NOS Valve Idle");
    engineStatusLogger.addColumn("NOS Valve Closed Switch High");
    engineStatusLogger.addColumn("NOS Valve Opened Switch High");
    engineStatusLogger.addColumn("NOS Valve Set %");
    engineStatusLogger.addColumn("IPA Valve State");
    engineStatusLogger.addColumn("IPA Valve Idle");
    engineStatusLogger.addColumn("IPA Valve Closed Switch High");
    engineStatusLogger.addColumn("IPA Valve Opened Switch High");
    engineStatusLogger.addColumn("IPA Valve Set %");
    engineStatusLogger.addColumn("Storage Error Status");
}

void PacketCSVLogging::initFillingStationStatusLogger() {
    if (!triedCreatingDataLogDir) {
        createDataLogDir();
    }

    fillingStationStatusLogger.openFile(dataLogDir + "/FillingStationStatus.log");
    fillingStationStatusLogger.addColumn("Timestamp");
    fillingStationStatusLogger.addColumn("Board State");
    fillingStationStatusLogger.addColumn("Fill Valve State");
    fillingStationStatusLogger.addColumn("Fill Valve Idle");
    fillingStationStatusLogger.addColumn("Fill Valve Closed Switch High");
    fillingStationStatusLogger.addColumn("Fill Valve Opened Switch High");
    fillingStationStatusLogger.addColumn("Fill Valve Set %");
    fillingStationStatusLogger.addColumn("Dump Valve State");
    fillingStationStatusLogger.addColumn("Dump Valve Idle");
    fillingStationStatusLogger.addColumn("Dump Valve Closed Switch High");
    fillingStationStatusLogger.addColumn("Dump Valve Opened Switch High");
    fillingStationStatusLogger.addColumn("Dump Valve Set %");
    fillingStationStatusLogger.addColumn("Storage Error Status");
}

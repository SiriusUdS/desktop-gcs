#include "PacketProcessing.h"

#include "BoardComStateMonitor.h"
#include "ComTask.h"
#include "DeviceInformation.h"
#include "GSDataCenter.h"
#include "LoadCell.h"
#include "Logging.h"
#include "PacketCSVLogging.h"
#include "PacketRateMonitor.h"
#include "PressureTransducer.h"
#include "SensorPlotData.h"
#include "SerialConfig.h"
#include "SwitchData.h"
#include "TemperatureSensor.h"
#include "UdpCom.h"
#include "ValveData.h"

#include "command/set_control_flag.hpp"
#include "framing/payload_type.hpp"
#include "response/response_type.hpp"
#include "system/board_id.hpp"
#include "system/valves/ecu.hpp"
#include "system/valves/fcu.hpp"
#include "telemetry/ecu_extended_system_state.hpp"
#include "telemetry/ecu_system_state.hpp"
#include "telemetry/fcu_extended_system_state.hpp"
#include "telemetry/fcu_system_state.hpp"
#include "telemetry/gs_system_state.hpp"
#include "telemetry/telemetry_type.hpp"

namespace PacketProcessing {
bool processIncomingSerialPacket();
bool processIncomingUdpPacket();
bool processUDPHeader(std::optional<UdpPacketMetadata> udpPacketMetadata, std::optional<std::vector<std::string>> logData);
bool processEngineTelemetryPacket(uint8_t* packetBuf);
bool processFillingStationTelemetryPacket(uint8_t* packetBuf);
bool processGSControlPacket(uint8_t* packetBuf);
bool processEngineStatusPacket(uint8_t* packetBuf);
bool processFillingStationStatusPacket(uint8_t* packetBuf);
bool routeDecodedSerialPacket();
bool routePacketByTypeUdp(UdpPacketMetadata udpMetadataOpt);
bool processSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta);
bool processExtendedSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta);
bool processGsSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta);
void computeThermistorValues(uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD], uint16_t boardId);
void computePressureSensorValues(uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD], uint16_t boardId);
void computeLoadCellValues(uint16_t loadCellAdcValues[GSDataCenterConfig::LOAD_CELL_AMOUNT]);
template <size_t N>
void addPlotData(std::array<SensorPlotData, N>& plotData, uint16_t* adcValues, float* computedValues, float timestamp);
bool validateIncomingPacketSize(size_t targetPacketSize, const char* packetName);
size_t packetSize{};
uint8_t serialPacketBuf[SerialConfig::MAX_PACKET_SIZE];
uint8_t udpPacketBuf[UdpConfig::UDPBufferSize];

// TODO: Think about declaring these arrays in their respective functions instead of declaring them globally to improve code clarity
float thermistorValues_C[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD]{};
float pressureSensorValues_psi[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD]{};
float loadCellValues_lb[GSDataCenterConfig::LOAD_CELL_AMOUNT]{};
} // namespace PacketProcessing


void PacketProcessing::processIncomingPackets() {
    // SERIAL PATH RETIRED — UDP-only for now; revive when serial returns.
    // while (ComTask::packetReceiver.packetAvailable()) {
    //     processIncomingSerialPacket();
    // }

    while (ComTask::udpPacketReceiver.packetAvailable()) {
        processIncomingUdpPacket();
    }
}

bool PacketProcessing::processIncomingUdpPacket() {
    std::optional<UdpPacketMetadata> udpMetadataOpt = ComTask::udpPacketReceiver.nextPacketMetadata();

    if (!udpMetadataOpt.has_value()) {
        return false;
    }

    packetSize = udpMetadataOpt->size;
    // Note: a zero-length payload is valid (no-payload responses such as Pong);
    // each handler validates its own expected size.

    if (!ComTask::udpPacketReceiver.getPacket(udpPacketBuf)) {
        return false;
    }
    processUDPHeader(udpMetadataOpt, std::nullopt);
    return routePacketByTypeUdp(udpMetadataOpt.value());
}

bool PacketProcessing::routePacketByTypeUdp(UdpPacketMetadata udpMetadataOpt) {
    // Telemetry is the only inbound payload class the GCS decodes today.
    if (udpMetadataOpt.payloadType == static_cast<uint8_t>(PayloadType::Telemetry)) {
        if (udpMetadataOpt.payloadID == static_cast<uint8_t>(TelemetryType::SystemState)) {
            return processSystemStatePacket(udpPacketBuf, udpMetadataOpt);
        }
        if (udpMetadataOpt.payloadID == static_cast<uint8_t>(TelemetryType::ExtendedSystemState)) {
            return processExtendedSystemStatePacket(udpPacketBuf, udpMetadataOpt);
        }
        GCS_APP_LOG_WARN("PacketProcessing: Unknown telemetry id {}, ignoring.", udpMetadataOpt.payloadID);
        return false;
    }

    if (udpMetadataOpt.payloadType == static_cast<uint8_t>(PayloadType::Response)) {
        if (udpMetadataOpt.payloadID == static_cast<uint8_t>(ResponseType::Pong)) {
            GSDataCenter::pongReceivedCount++;
            GSDataCenter::lastPongSenderId = static_cast<uint8_t>(udpMetadataOpt.deviceID);
            return true;
        }
        if (udpMetadataOpt.payloadID == static_cast<uint8_t>(ResponseType::Ack)) {
            GCS_APP_LOG_INFO("PacketProcessing: Ack received from board {} (seq {}).",
                             udpMetadataOpt.deviceID, static_cast<unsigned>(udpMetadataOpt.seq));
            return true;
        }
        GCS_APP_LOG_WARN("PacketProcessing: Unknown response id {}, ignoring.", udpMetadataOpt.payloadID);
        return false;
    }

    GCS_APP_LOG_WARN("PacketProcessing: Unhandled UDP payload type {}, ignoring.", udpMetadataOpt.payloadType);
    return false;
}

#if 0 // SERIAL PATH RETIRED — UDP-only for now; revive when serial returns.
bool PacketProcessing::processIncomingSerialPacket() {
    std::optional<PacketMetadata> packetMetadataOpt = ComTask::packetReceiver.nextPacketMetadata();

    if (!packetMetadataOpt.has_value()) {
        // No available packets
        return false;
    }

    packetSize = packetMetadataOpt->size;

    if (packetSize < sizeof(TelemetryHeader)) {
        GCS_APP_LOG_WARN("PacketProcessing: Received packet size ({}) too small to fit header ({}), ignoring packet.",
                         packetSize,
                         sizeof(TelemetryHeader));
        ComTask::packetReceiver.dumpNextPacket();
        return false;
    } else if (packetSize > SerialConfig::MAX_PACKET_SIZE) {
        GCS_APP_LOG_WARN("PacketProcessing: Received packet size ({}) too big to fit in packet buffer ({}), ignoring packet.",
                         packetSize,
                         SerialConfig::MAX_PACKET_SIZE);
        ComTask::packetReceiver.dumpNextPacket();
        return false;
    }

    if (!ComTask::com->getPacket(serialPacketBuf)) {
        GCS_APP_LOG_ERROR("PacketProcessing: Something went wrong while getting the next packet.");
        return false;
    }

    return routeDecodedSerialPacket();
}

bool routeByPacketTypeSerial(TelemetryHeader* header, bool& value1, uint8_t* packetBuf) {
    switch (header->bits.type) {
    case TELEMETRY_TYPE_CODE:
        if (header->bits.boardId == static_cast<uint8_t>(BoardId::Engine)) {
            value1 = PacketProcessing::processEngineTelemetryPacket(packetBuf);
            return true;
        } else if (header->bits.boardId == static_cast<uint8_t>(BoardId::FillingStation)) {
            value1 = PacketProcessing::processFillingStationTelemetryPacket(packetBuf);
            return true;
        } else if (header->bits.boardId == static_cast<uint8_t>(BoardId::GsControl)) {
            GCS_APP_LOG_WARN("PacketProcessing: Tried processing GS control telemetry packet, but that doesn't exist.");
            value1 = false;
            return true;
        } else {
            GCS_APP_LOG_WARN("PacketProcessing: Telemetry packet contains invalid boardId, ignoring packet.");
            value1 = false;
            return true;
        }
    case STATUS_TYPE_CODE:
        if (header->bits.boardId == static_cast<uint8_t>(BoardId::Engine)) {
            value1 = PacketProcessing::processEngineStatusPacket(packetBuf);
            return true;
        } else if (header->bits.boardId == static_cast<uint8_t>(BoardId::FillingStation)) {
            value1 = PacketProcessing::processFillingStationStatusPacket(packetBuf);
            return true;
        } else if (header->bits.boardId == static_cast<uint8_t>(BoardId::GsControl)) {
            value1 = PacketProcessing::processGSControlPacket(packetBuf);
            return true;
        } else {
            GCS_APP_LOG_WARN("PacketProcessing: Status packet contains invalid boardId, ignoring packet.");
            value1 = false;
            return true;
        }
    }
    return false;
}

bool PacketProcessing::routeDecodedSerialPacket() {
    TelemetryHeader* header;
    header = reinterpret_cast<TelemetryHeader*>(serialPacketBuf);
    bool valid;
    if (routeByPacketTypeSerial(header, valid, serialPacketBuf))
        return valid;

    GCS_APP_LOG_ERROR("PacketProcessing: Unknown packet type, ignoring packet.");
    return false;
}

#endif // SERIAL PATH RETIRED

bool PacketProcessing::processUDPHeader(std::optional<UdpPacketMetadata> udpPacketMetadata, std::optional<std::vector<std::string>> logData) {
    if (!udpPacketMetadata.has_value()) {
        return false;
    }

    DeviceInformation deviceInformation;
    deviceInformation.deviceID = udpPacketMetadata->deviceID;
    deviceInformation.deviceStatus = udpPacketMetadata->deviceState;
    deviceInformation.deviceTsMs = udpPacketMetadata->deviceTsMs;
    deviceInformation.deviceCtrlFlags = udpPacketMetadata->deviceCtrlFlags;
    if (logData.has_value()) {
        for (const auto& log : logData.value()) {
            deviceInformation.deviceLogs.push_back(log);
        }
    }

    ComTask::updateUDPDevice(deviceInformation);
    return true;
}

namespace {
// Map a common-protocol ValveInfo telemetry record onto the GCS's ValveData.
// Measured open-% is no longer on the wire; current_set_value (the commanded %)
// stands in for positionOpened_pct per the migration decision.
void updateValveData(ValveData& dst, const ValveInfo& src) {
    dst.state = static_cast<uint8_t>(src.state);
    dst.isIdle = !src.status.in_transition; // moving toward a target => not idle
    dst.closedSwitchHigh = src.status.closed_limit_high;
    dst.openedSwitchHigh = src.status.open_limit_high;
    dst.positionOpened_pct = src.current_set_value;
}

// Decode one engine (ECU) SystemState record into GSDataCenter + the CSV log.
void decodeEngineState(const SystemStateBase& base, uint8_t boardState) {
    GSDataCenter::motorBoardState = boardState;
    GSDataCenter::motorBoardTimestamp_ms = base.creation_timestamp_ms;
    GSDataCenter::motorBoardStorageErrorStatus = static_cast<uint16_t>(base.storage_info.status.error);
    updateValveData(GSDataCenter::nosValveData, base.valve_info[static_cast<size_t>(EcuValves::NOS)]);
    updateValveData(GSDataCenter::ipaValveData, base.valve_info[static_cast<size_t>(EcuValves::IPA)]);
    GSDataCenter::motorAdcAverager.submit(base.adc_info.channels);
    PacketCSVLogging::logEngineStatus(static_cast<float>(base.creation_timestamp_ms),
                                      boardState,
                                      base.valve_info[static_cast<size_t>(EcuValves::NOS)],
                                      base.valve_info[static_cast<size_t>(EcuValves::IPA)],
                                      static_cast<uint16_t>(base.storage_info.status.error),
                                      base.adc_info.channels);
}

// Decode one filling-station (FCU) SystemState record into GSDataCenter + the CSV log.
void decodeFillingStationState(const SystemStateBase& base, uint8_t boardState) {
    GSDataCenter::fillingStationBoardState = boardState;
    GSDataCenter::fillingStationBoardTimestamp_ms = base.creation_timestamp_ms;
    GSDataCenter::fillingStationBoardStorageErrorStatus = static_cast<uint16_t>(base.storage_info.status.error);
    updateValveData(GSDataCenter::fillValveData, base.valve_info[static_cast<size_t>(FcuValves::Fill)]);
    updateValveData(GSDataCenter::dumpValveData, base.valve_info[static_cast<size_t>(FcuValves::Dump)]);
    GSDataCenter::fillingStationAdcAverager.submit(base.adc_info.channels);
    PacketCSVLogging::logFillingStationStatus(static_cast<float>(base.creation_timestamp_ms),
                                              boardState,
                                              base.valve_info[static_cast<size_t>(FcuValves::Fill)],
                                              base.valve_info[static_cast<size_t>(FcuValves::Dump)],
                                              static_cast<uint16_t>(base.storage_info.status.error),
                                              base.adc_info.channels);
}

// Decode one low-rate ECU ExtendedSystemState record (just the live control-flag
// bitmask for now; event timestamps will land here later).
void decodeEcuExtendedState(const EcuExtendedSystemState& rec) {
    GSDataCenter::motorBoardControlFlags =
        rec.base.control_flags_base | (static_cast<uint32_t>(rec.base.control_flags_board) << CONTROL_FLAG_BOARD_OFFSET);
}

// Decode one low-rate FCU ExtendedSystemState record: control flags + the 4
// thermocouple channels (raw code + linearized degC) into GSDataCenter plots.
void decodeFcuExtendedState(const FcuExtendedSystemState& rec) {
    GSDataCenter::fillingStationBoardControlFlags =
        rec.base.control_flags_base | (static_cast<uint32_t>(rec.base.control_flags_board) << CONTROL_FLAG_BOARD_OFFSET);

    const float timestamp = static_cast<float>(rec.base.creation_timestamp_ms);
    for (size_t i = 0; i < GSDataCenterConfig::THERMOCOUPLE_AMOUNT; i++) {
        const ThermocoupleInfo& tc = rec.thermocouple_info[i];
        const float tempC = static_cast<float>(tc.thermocouple_code) / 128.0f; // LSB = 2^-7 degC
        GSDataCenter::Thermocouple_FillingStation_PlotData.data[i].addData(static_cast<float>(tc.thermocouple_code), tempC, timestamp);
        GSDataCenter::fillingStationThermocouple_C[i] = tempC;
        GSDataCenter::fillingStationThermocoupleState[i] = static_cast<uint8_t>(tc.state);
    }
}
} // namespace

bool PacketProcessing::processSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta) {
    // The payload is a batch of N back-to-back SystemState records (N = 1..~50),
    // ordered oldest-to-newest. Each record is decoded and CSV-logged; GSDataCenter
    // ends up holding the most recent (last) record. Board state rides in the
    // EthernetHeader (sender_state) and is shared by the whole batch.
    const BoardId board = static_cast<BoardId>(meta.deviceID);
    const uint8_t boardState = meta.deviceState;

    if (board == BoardId::Engine) {
        constexpr size_t recordSize = sizeof(EcuSystemState);
        if (packetSize == 0 || packetSize % recordSize != 0) {
            GCS_APP_LOG_WARN("PacketProcessing: EcuSystemState batch ({} B) is not a whole multiple of {} B, ignoring.", packetSize, recordSize);
            return false;
        }
        const EcuSystemState* records = reinterpret_cast<const EcuSystemState*>(payloadBuf);
        const size_t recordCount = packetSize / recordSize;
        for (size_t i = 0; i < recordCount; i++) {
            decodeEngineState(records[i].base, boardState);
        }
        GSDataCenter::latestEcuSystemState.store(records[recordCount - 1]); // newest in the batch, for the full-field UI
        GSDataCenter::ecuSystemStateCount += recordCount;

        ComTask::packetRateMonitor.trackPacket();
        ComTask::motorBoardComStateMonitor.trackSuccessfulPacketRead();
    } else if (board == BoardId::FillingStation) {
        constexpr size_t recordSize = sizeof(FcuSystemState);
        if (packetSize == 0 || packetSize % recordSize != 0) {
            GCS_APP_LOG_WARN("PacketProcessing: FcuSystemState batch ({} B) is not a whole multiple of {} B, ignoring.", packetSize, recordSize);
            return false;
        }
        const FcuSystemState* records = reinterpret_cast<const FcuSystemState*>(payloadBuf);
        const size_t recordCount = packetSize / recordSize;
        for (size_t i = 0; i < recordCount; i++) {
            decodeFillingStationState(records[i].base, boardState);
        }
        GSDataCenter::latestFcuSystemState.store(records[recordCount - 1]); // newest in the batch, for the full-field UI
        GSDataCenter::fcuSystemStateCount += recordCount;

        ComTask::packetRateMonitor.trackPacket();
        ComTask::fillingStationBoardComStateMonitor.trackSuccessfulPacketRead();
    } else if (board == BoardId::GsControl) {
        // The GS-control board (id 3) reports its panel state under the SystemState telemetry id;
        // decode it as a GSSystemState rather than an ECU/FCU record.
        return processGsSystemStatePacket(payloadBuf, meta);
    } else {
        GCS_APP_LOG_WARN("PacketProcessing: SystemState from unsupported board id {}, ignoring.", meta.deviceID);
        return false;
    }

    // TODO(ADC): base.adc_info carries 8 signed int32 channels. The per-board
    // channel->sensor map and the int32->engineering-value conversion are not yet
    // confirmed by firmware, and the 8-channel layout does not fit the current
    // GSDataCenterConfig counts (8 thermistor + 2 pressure + 2 load cell). Plot
    // wiring (computeThermistor/Pressure/LoadCellValues + addPlotData) is deferred.
    // TODO(commands): ignite/launch timestamps, lastReceivedCommandCode and
    // timeSinceLastCommand are not in SystemStateBase; those fields stay unfed.
    // TODO(GS-control): GSControl switch states are intentionally not decoded yet
    // (no SystemState source); the GS-control telemetry is expected to return soon.
    return true;
}

bool PacketProcessing::processExtendedSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta) {
    // Low-rate (~10 Hz) batch of N ExtendedSystemState records, routed per board.
    const BoardId board = static_cast<BoardId>(meta.deviceID);

    if (board == BoardId::Engine) {
        constexpr size_t recordSize = sizeof(EcuExtendedSystemState);
        if (packetSize == 0 || packetSize % recordSize != 0) {
            GCS_APP_LOG_WARN("PacketProcessing: EcuExtendedSystemState batch ({} B) is not a whole multiple of {} B, ignoring.", packetSize, recordSize);
            return false;
        }
        const EcuExtendedSystemState* records = reinterpret_cast<const EcuExtendedSystemState*>(payloadBuf);
        const size_t recordCount = packetSize / recordSize;
        for (size_t i = 0; i < recordCount; i++) {
            decodeEcuExtendedState(records[i]);
        }
        GSDataCenter::latestEcuExtendedSystemState.store(records[recordCount - 1]); // newest in the batch, for the full-field UI
        GSDataCenter::ecuExtendedSystemStateCount += recordCount;
        ComTask::packetRateMonitor.trackPacket();
        ComTask::motorBoardComStateMonitor.trackSuccessfulPacketRead();
    } else if (board == BoardId::FillingStation) {
        constexpr size_t recordSize = sizeof(FcuExtendedSystemState);
        if (packetSize == 0 || packetSize % recordSize != 0) {
            GCS_APP_LOG_WARN("PacketProcessing: FcuExtendedSystemState batch ({} B) is not a whole multiple of {} B, ignoring.", packetSize, recordSize);
            return false;
        }
        const FcuExtendedSystemState* records = reinterpret_cast<const FcuExtendedSystemState*>(payloadBuf);
        const size_t recordCount = packetSize / recordSize;
        for (size_t i = 0; i < recordCount; i++) {
            decodeFcuExtendedState(records[i]);
        }
        GSDataCenter::latestFcuExtendedSystemState.store(records[recordCount - 1]); // newest in the batch, for the full-field UI
        GSDataCenter::fcuExtendedSystemStateCount += recordCount;
        ComTask::packetRateMonitor.trackPacket();
        ComTask::fillingStationBoardComStateMonitor.trackSuccessfulPacketRead();
    } else {
        GCS_APP_LOG_WARN("PacketProcessing: ExtendedSystemState from unsupported board id {}, ignoring.", meta.deviceID);
        return false;
    }
    return true;
}

bool PacketProcessing::processGsSystemStatePacket(uint8_t* payloadBuf, const UdpPacketMetadata& meta) {
    // Batch of N back-to-back GSSystemState records (the ground-station panel state). Unlike the
    // ECU/FCU SystemState there is a single source, so no per-board routing — keep the newest record.
    constexpr size_t recordSize = sizeof(GSSystemState);
    if (packetSize == 0 || packetSize % recordSize != 0) {
        GCS_APP_LOG_WARN("PacketProcessing: GSSystemState batch ({} B) is not a whole multiple of {} B, ignoring.", packetSize, recordSize);
        return false;
    }
    const GSSystemState* records = reinterpret_cast<const GSSystemState*>(payloadBuf);
    const size_t recordCount = packetSize / recordSize;
    GSDataCenter::latestGsSystemState.store(records[recordCount - 1]); // newest in the batch, for the UI table
    GSDataCenter::gsSystemStateCount += recordCount;
    ComTask::packetRateMonitor.trackPacket();
    return true;
}


#if 0 // SERIAL PATH RETIRED — UDP-only for now; revive when serial returns.
bool PacketProcessing::processEngineTelemetryPacket(uint8_t* packetBuf) {
    if (!validateIncomingPacketSize(sizeof(EngineTelemetryPacket), "EngineTelemetryPacket")) {
        return false;
    }

    EngineTelemetryPacket* packet = reinterpret_cast<EngineTelemetryPacket*>(packetBuf);

    if (!isPacketIntegrityValid(packetBuf, packet, sizeof(EngineTelemetryPacket))) {
        return false;
    }

    float timestamp = static_cast<float>(packet->fields.timestamp_ms);
    uint16_t* adcValues = packet->fields.adcValues;

    uint16_t* thermistorAdcValues = adcValues + SerialConfig::THERMISTOR_ADC_VALUES_INDEX_OFFSET;
    uint16_t* pressureSensorAdcValues = adcValues + SerialConfig::PRESSURE_SENSOR_ADC_VALUES_INDEX_OFFSET;

    computeThermistorValues(thermistorAdcValues, static_cast<uint8_t>(BoardId::Engine));
    computePressureSensorValues(pressureSensorAdcValues, static_cast<uint8_t>(BoardId::Engine));

    addPlotData<GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD>(GSDataCenter::Thermistor_Motor_PlotData.data,
                                                                 thermistorAdcValues,
                                                                 thermistorValues_C,
                                                                 timestamp);
    addPlotData<GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD>(GSDataCenter::PressureSensor_Motor_PlotData.data,
                                                                      pressureSensorAdcValues,
                                                                      pressureSensorValues_psi,
                                                                      timestamp);

    ComTask::packetRateMonitor.trackPacket();
    ComTask::engineTelemetryPacketRateMonitor.trackPacket();
    ComTask::motorBoardComStateMonitor.trackSuccessfulPacketRead();

    PacketCSVLogging::logEngineTelemetryPacket(timestamp, thermistorAdcValues, thermistorValues_C, pressureSensorAdcValues, pressureSensorValues_psi);
    return true;
}

#endif // SERIAL PATH RETIRED


#if 0 // SERIAL PATH RETIRED — UDP-only for now; revive when serial returns.
bool PacketProcessing::processFillingStationTelemetryPacket(uint8_t* packetBuf) {
    if (!validateIncomingPacketSize(sizeof(FillingStationTelemetryPacket), "FillingStationTelemetryPacket")) {
        return false;
    }

    FillingStationTelemetryPacket* packet = reinterpret_cast<FillingStationTelemetryPacket*>(packetBuf);

    if (!isPacketIntegrityValid(packetBuf, packet, sizeof(FillingStationTelemetryPacket))) {
        return false;
    }

    float timestamp = static_cast<float>(packet->fields.timestamp_ms);
    uint16_t* adcValues = packet->fields.adcValues;

    uint16_t* thermistorAdcValues = adcValues + SerialConfig::THERMISTOR_ADC_VALUES_INDEX_OFFSET;
    uint16_t* pressureSensorAdcValues = adcValues + SerialConfig::PRESSURE_SENSOR_ADC_VALUES_INDEX_OFFSET;
    uint16_t* loadCellAdcValues = adcValues + SerialConfig::LOAD_CELL_ADC_VALUES_INDEX_OFFSET;

    computeThermistorValues(thermistorAdcValues, static_cast<uint8_t>(BoardId::Engine));
    computePressureSensorValues(pressureSensorAdcValues, static_cast<uint8_t>(BoardId::FillingStation));
    computeLoadCellValues(loadCellAdcValues);

    addPlotData<GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD>(GSDataCenter::Thermistor_FillingStation_PlotData.data,
                                                                 thermistorAdcValues,
                                                                 thermistorValues_C,
                                                                 timestamp);
    addPlotData<GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD>(GSDataCenter::PressureSensor_FillingStation_PlotData.data,
                                                                      pressureSensorAdcValues,
                                                                      pressureSensorValues_psi,
                                                                      timestamp);
    addPlotData<GSDataCenterConfig::LOAD_CELL_AMOUNT>(GSDataCenter::LoadCell_FillingStation_PlotData.data,
                                                      loadCellAdcValues,
                                                      loadCellValues_lb,
                                                      timestamp);

    ComTask::packetRateMonitor.trackPacket();
    ComTask::fillingStationTelemetryPacketRateMonitor.trackPacket();
    ComTask::fillingStationBoardComStateMonitor.trackSuccessfulPacketRead();

    PacketCSVLogging::logFillingStationTelemetryPacket(timestamp,
                                                       thermistorAdcValues,
                                                       thermistorValues_C,
                                                       pressureSensorAdcValues,
                                                       pressureSensorValues_psi,
                                                       loadCellAdcValues,
                                                       loadCellValues_lb);
    return true;
}

#endif // SERIAL PATH RETIRED


#if 0 // SERIAL PATH RETIRED — UDP-only for now; revive when serial returns.
bool PacketProcessing::processGSControlPacket(uint8_t* packetBuf) {
    if (!validateIncomingPacketSize(sizeof(GSControlStatusPacket), "GSControlStatusPacket")) {
        return false;
    }

    GSControlStatusPacket* packet = reinterpret_cast<GSControlStatusPacket*>(packetBuf);

    if (!isPacketIntegrityValid(packetBuf, packet, sizeof(GSControlStatusPacket))) {
        return false;
    }

    GSControlStatus& status = packet->fields.status;

    GSDataCenter::AllowDumpSwitchData.isOn = status.bits.isAllowDumpSwitchOn;
    GSDataCenter::AllowFillSwitchData.isOn = status.bits.isAllowFillSwitchOn;
    GSDataCenter::ArmIgniterSwitchData.isOn = status.bits.isArmIgniterSwitchOn;
    GSDataCenter::ArmServoSwitchData.isOn = status.bits.isArmServoSwitchOn;
    GSDataCenter::EmergencyStopButtonData.isOn = status.bits.isEmergencyStopButtonPressed;
    GSDataCenter::FireIgniterButtonData.isOn = status.bits.isFireIgniterButtonPressed;
    GSDataCenter::UnsafeKeySwitchData.isOn = status.bits.isUnsafeKeySwitchPressed;
    GSDataCenter::ValveStartButtonData.isOn = status.bits.isValveStartButtonPressed;

    GSDataCenter::lastReceivedGSCommandTimestamp_ms = packet->fields.lastReceivedGSCommandTimestamp_ms;
    GSDataCenter::lastBoardSentCommandCode = packet->fields.lastBoardSentCommandCode;
    GSDataCenter::lastSentCommandTimestamp_ms = packet->fields.lastSentCommandTimestamp_ms;

    GSDataCenter::gsControlBoardState = status.bits.state;

    ComTask::packetRateMonitor.trackPacket();
    ComTask::gsControlPacketRateMonitor.trackPacket();
    ComTask::gsControlBoardComStateMonitor.trackSuccessfulPacketRead();

    PacketCSVLogging::logGSControlPacket(packet);
    return true;
}

bool PacketProcessing::processEngineStatusPacket(uint8_t* packetBuf) {
    if (!validateIncomingPacketSize(sizeof(EngineStatusPacket), "EngineStatusPacket")) {
        return false;
    }
    EngineStatusPacket* packet = reinterpret_cast<EngineStatusPacket*>(packetBuf);;

    if (!isPacketIntegrityValid(packetBuf, packet, sizeof(EngineStatusPacket))) {
        return false;
    }

    ValveStatus& nosValveStatus = packet->fields.valveStatus[SerialConfig::NOS_VALVE_STATUS_INDEX];
    ValveStatus& ipaValveStatus = packet->fields.valveStatus[SerialConfig::IPA_VALVE_STATUS_INDEX];

    GSDataCenter::nosValveData.isIdle = nosValveStatus.bits.isIdle;
    GSDataCenter::nosValveData.closedSwitchHigh = nosValveStatus.bits.closedSwitchHigh;
    GSDataCenter::nosValveData.openedSwitchHigh = nosValveStatus.bits.openedSwitchHigh;

    GSDataCenter::ipaValveData.isIdle = ipaValveStatus.bits.isIdle;
    GSDataCenter::ipaValveData.closedSwitchHigh = ipaValveStatus.bits.closedSwitchHigh;
    GSDataCenter::ipaValveData.openedSwitchHigh = ipaValveStatus.bits.openedSwitchHigh;

    GSDataCenter::igniteTimestamp_ms = packet->fields.igniteTimestamp_ms;
    GSDataCenter::launchTimestamp_ms = packet->fields.launchTimestamp_ms;
    GSDataCenter::timeSinceLastCommandMotorBoard_ms = packet->fields.timeSinceLastCommand_ms;
    GSDataCenter::lastReceivedCommandCodeMotorBoard = packet->fields.lastReceivedCommandCode;

    GSDataCenter::motorBoardState = packet->fields.status.bits.state;
    GSDataCenter::motorBoardStorageErrorStatus = packet->fields.storageErrorStatus.value;

    ComTask::packetRateMonitor.trackPacket();
    ComTask::engineStatusPacketRateMonitor.trackPacket();
    ComTask::motorBoardComStateMonitor.trackSuccessfulPacketRead();

    PacketCSVLogging::logEngineStatusPacket(packet);
    return true;
}

bool PacketProcessing::processFillingStationStatusPacket(uint8_t* packetBuf) {
    if (!validateIncomingPacketSize(sizeof(FillingStationStatusPacket), "FillingStationStatusPacket")) {
        return false;
    }

    FillingStationStatusPacket* packet = reinterpret_cast<FillingStationStatusPacket*>(packetBuf);

    if (!isPacketIntegrityValid(packetBuf, packet, sizeof(FillingStationStatusPacket))) {
        return false;
    }

    ValveStatus& fillValveStatus = packet->fields.valveStatus[SerialConfig::FILL_VALVE_STATUS_INDEX];
    ValveStatus& dumpValveStatus = packet->fields.valveStatus[SerialConfig::DUMP_VALVE_STATUS_INDEX];

    GSDataCenter::fillValveData.isIdle = fillValveStatus.bits.isIdle;
    GSDataCenter::fillValveData.closedSwitchHigh = fillValveStatus.bits.closedSwitchHigh;
    GSDataCenter::fillValveData.openedSwitchHigh = fillValveStatus.bits.openedSwitchHigh;
    GSDataCenter::fillValveData.positionOpened_pct = fillValveStatus.bits.positionOpened_pct;

    GSDataCenter::dumpValveData.isIdle = dumpValveStatus.bits.isIdle;
    GSDataCenter::dumpValveData.closedSwitchHigh = dumpValveStatus.bits.closedSwitchHigh;
    GSDataCenter::dumpValveData.openedSwitchHigh = dumpValveStatus.bits.openedSwitchHigh;
    GSDataCenter::dumpValveData.positionOpened_pct = dumpValveStatus.bits.positionOpened_pct;

    GSDataCenter::timeSinceLastCommandFillingStationBoard_ms = packet->fields.timeSinceLastCommand_ms;
    GSDataCenter::lastReceivedCommandCodeFillingStationBoard = packet->fields.lastReceivedCommandCode;

    GSDataCenter::fillingStationBoardState = packet->fields.status.bits.state;
    GSDataCenter::fillingStationBoardStorageErrorStatus = packet->fields.storageErrorStatus.value;

    ComTask::packetRateMonitor.trackPacket();
    ComTask::fillingStationStatusPacketRateMonitor.trackPacket();
    ComTask::fillingStationBoardComStateMonitor.trackSuccessfulPacketRead();

    PacketCSVLogging::logFillingStationStatusPacket(packet);
    return true;
}

#endif // SERIAL PATH RETIRED

void PacketProcessing::computeThermistorValues(uint16_t thermistorAdcValues[GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD], uint16_t boardId) {
    for (size_t i = 0; i < GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD; i++) {
        float adcValue = static_cast<float>(thermistorAdcValues[i]);
        thermistorValues_C[i] = TemperatureSensor::adcToTemperature_C(adcValue);
    }
}

void PacketProcessing::computePressureSensorValues(uint16_t pressureSensorAdcValues[GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD],
                                                   uint16_t boardId) {
    uint8_t indexOffset = boardId == static_cast<uint8_t>(BoardId::Engine) ? 2 : 0;
    for (size_t i = 0; i < GSDataCenterConfig::PRESSURE_SENSOR_AMOUNT_PER_BOARD; i++) {
        float adcValue = static_cast<float>(pressureSensorAdcValues[i]);
        uint16_t sensorIndex = static_cast<uint16_t>(i + indexOffset);
        pressureSensorValues_psi[i] = PressureTransducer::adcToPressure_psi(adcValue, sensorIndex);
    }
}

void PacketProcessing::computeLoadCellValues(uint16_t loadCellAdcValues[GSDataCenterConfig::LOAD_CELL_AMOUNT]) {
    for (size_t i = 0; i < GSDataCenterConfig::LOAD_CELL_AMOUNT; i++) {
        float adcValue = static_cast<float>(loadCellAdcValues[i]);
        loadCellValues_lb[i] = LoadCell::adcToWeight_lb(adcValue, 0); // TODO: Change this index later?
    }
}

template <size_t N>
void PacketProcessing::addPlotData(std::array<SensorPlotData, N>& plotData, uint16_t* adcValues, float* computedValues, float timestamp) {
    for (size_t i = 0; i < N; i++) {
        float adcValue = static_cast<float>(adcValues[i]);
        plotData[i].addData(adcValue, computedValues[i], timestamp);
    }
}

bool PacketProcessing::validateIncomingPacketSize(size_t targetPacketSize, const char* packetName) {
    if (packetSize != targetPacketSize) {
        GCS_APP_LOG_WARN("PacketProcessing: Invalid {} size ({}), expected size ({}), ignoring packet.", packetName, packetSize, targetPacketSize);
        return false;
    }
    return true;
}

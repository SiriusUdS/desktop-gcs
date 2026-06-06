#pragma once
#include "UdpPacket.h"
#include "Engine/EngineErrorStatus.h"
#include "Engine/EngineSensors.h"
#include "Engine/EngineStatus.h"
#include "FillingStation/FillingStationErrorStatus.h"
#include "FillingStation/FillingStationSensors.h"
#include "FillingStation/FillingStationStatus.h"
#include "Storage/StorageErrorStatus.h"
#include "Storage/StorageStatus.h"
#include "Valve/ValveStatus.h"

//Engine Payload
typedef struct {
    networking::UDPPacketHeader header;
    uint16_t adcValues[ENGINE_ADC_CHANNEL_AMOUNT];
    EngineStatus status;
    EngineErrorStatus errorStatus;
    ValveStatus valveStatus[ENGINE_VALVE_AMOUNT];
    StorageStatus storageStatus;
    StorageErrorStatus storageErrorStatus;
    uint32_t igniteTimestamp_ms;
    uint32_t launchTimestamp_ms;
    uint32_t timeSinceLastCommand_ms;
    uint32_t lastReceivedCommandCode;
    uint32_t crc;
}
EngineTelemetryUDPPacketFields;

typedef union {
    EngineTelemetryUDPPacketFields fields;
    uint8_t bytes[sizeof(EngineTelemetryUDPPacketFields)/sizeof(uint32_t)];
    uint8_t data[sizeof(EngineTelemetryUDPPacketFields)];
} EngineTelemetryUDPPacket;

//FillStation Payload
typedef struct {
    networking::UDPPacketHeader header;
    uint16_t adcValues[FILLING_STATION_ADC_CHANNEL_AMOUNT];
    FillingStationStatus status;
    FillingStationErrorStatus errorStatus;
    ValveStatus valveStatus[FILLING_STATION_VALVE_AMOUNT];
    StorageStatus storageStatus;
    StorageErrorStatus storageErrorStatus;
    uint32_t igniteTimestamp_ms;
    uint32_t timeSinceLastCommand_ms;
    uint32_t lastReceivedCommandCode;
    uint32_t crc;
}
FillStationTelemetryUDPFields;

typedef union {
    FillStationTelemetryUDPFields fields;
    uint8_t bytes[sizeof(FillStationTelemetryUDPFields)/sizeof(uint32_t)];
    uint8_t data[sizeof(FillStationTelemetryUDPFields)];
} FillStationTelemetryUDPPacket;

//GSControl Payload
typedef struct {
    networking::UDPPacketHeader header;
    GSControlStatus status;
    GSControlErrorStatus errorStatus;
    uint32_t lastReceivedGSCommandTimestamp_ms;
    uint32_t lastBoardSentCommandCode;
    uint32_t lastSentCommandTimestamp_ms;
    uint32_t crc;
}
GSControlUdpFields;

typedef union {
    GSControlUdpFields fields;
    
    uint32_t bytes[sizeof(GSControlUdpFields)/sizeof(uint32_t)];
    
    uint8_t data[sizeof(GSControlUdpFields)];
}
GSControlUdpPacket;
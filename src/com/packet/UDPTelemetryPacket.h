#pragma once
#include "UdpPacket.h"
#include "Engine/EngineErrorStatus.h"
#include "Engine/EngineSensors.h"
#include "Engine/EngineStatus.h"
#include "FillingStation/FillingStationErrorStatus.h"
#include "FillingStation/FillingStationSensors.h"
#include "FillingStation/FillingStationStatus.h"
#include "GSControl/GSControlStatus.h"
#include "GSControl/GSControlErrorStatus.h"
#include "Storage/StorageErrorStatus.h"
#include "Storage/StorageStatus.h"
#include "Valve/ValveStatus.h"

#define SYNC_PACKET_CODE     (uint8_t)0x81
#define DIAGNOSE_PACKET_CODE (uint8_t)0x02
#define REQUEST_STATE        (uint8_t)0x83
#define GET_SYSTEM           (uint8_t)0x03
#define SET_SERVO            (uint8_t)0x84
#define GET_SERVO            (uint8_t)0x04

//Servos
#define SERVO_OPEN   (1 << 0)
#define SERVO_CLOSE  (1 << 1)
#define SERVO_MANUAL (1 << 2)

//Request state
#define RS_FLAG_GET_SYSTEM_AFTER (1 << 0)
#define RS_FLAG_RESET            (1 << 1)
#define RS_FLAG_START_LOGGING    (1 << 2)
#define RS_FLAG_SPREAD           (1 << 3)

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

//Channel
typedef union{
    struct {
        uint32_t id: 8;
        uint32_t value: 24;
    };
    uint32_t code;
} Channel;

//Interface Field
typedef union {
    struct {
        uint8_t initalized: 1;
        uint8_t readingError: 1;
        uint8_t writingError: 1;
        uint8_t deviceNotFound: 1;
        uint8_t invalidState: 1;
        uint8_t crcError: 1;
        uint8_t reserved: 2;
    } bits;
    uint8_t value;
} InterfaceFieldFlags;

typedef struct {
    InterfaceFieldFlags canFlags;
    InterfaceFieldFlags ethernetFlags;
    InterfaceFieldFlags sdCardFlags;
    InterfaceFieldFlags valve1Falgs;
    InterfaceFieldFlags valve2Falgs;
    InterfaceFieldFlags igniterFlags;
    uint16_t reserved;
    uint32_t erno;
}InterfaceFrame;

typedef union {
    InterfaceFrame frame;
    uint8_t byteMap[sizeof(InterfaceFrame)];
}InterfaceField;

//SystemPacket Payload
typedef struct {
    networking::UDPPacketHeader header;
    uint32_t frameTs_MS;
    uint32_t lastHandshakeTs_MS;
    InterfaceField interfaces;
    uint8_t byteMap[sizeof(InterfaceFrame)];
    uint32_t crc;
} FrameSystemPacket;

typedef union {
    FrameSystemPacket frame;
    
    uint32_t bytes[sizeof(FrameSystemPacket)/sizeof(uint32_t)];;
    
    uint8_t data[sizeof(FrameSystemPacket)];
} SystemPacket;

//Servo Packet
typedef struct {
    networking::UDPPacketHeader header;
    uint8_t status;
    uint8_t reserved;
    uint16_t value;
    uint32_t crc;
} FrameServo;

typedef union {
    FrameServo frame;
    uint32_t bytes[sizeof(FrameServo)/sizeof(uint32_t)];
    uint8_t data[sizeof(FrameServo)];
} ServoPacket;
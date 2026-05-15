#include "CRC.h"
#include "GSControl/GSControlErrorStatus.h"
#include "GSControl/GSControlStatus.h"
#include "Telecommunication/BoardCommand.h"
#include "Telecommunication/PacketHeaderVariable.h"
#include "Telecommunication/TelemetryPacket.h"

#include <doctest.h>

TEST_CASE("CRC should work with engine telemetry packet") {
    EngineTelemetryPacket packet;
    packet.fields.header.bits.type = TELEMETRY_TYPE_CODE;
    packet.fields.header.bits.boardId = ENGINE_BOARD_ID;
    packet.fields.header.bits.RESERVED = 0;
    packet.fields.timestamp_ms = 1465975;

    uint16_t adcValues[ENGINE_ADC_CHANNEL_AMOUNT] = {3, 2, 2, 2, 2, 2, 2, 2, 121, 180, 92, 800, 4095, 1136, 19, 6};
    for (size_t i = 0; i < ENGINE_ADC_CHANNEL_AMOUNT; i++) {
        packet.fields.adcValues[i] = adcValues[i];
    }

    const uint32_t expectedCRC = 4112858198u;
    const uint32_t computedCRC = CRC::computeCrc(packet.data, sizeof(packet) - sizeof(packet.fields.crc));
    CHECK(expectedCRC == computedCRC);
}

TEST_CASE("CRC should work with GS control status packet") {
    GSControlStatusPacket packet;
    packet.fields.header.bits.type = STATUS_TYPE_CODE;
    packet.fields.header.bits.boardId = GS_CONTROL_BOARD_ID;
    packet.fields.header.bits.RESERVED = 0;
    packet.fields.timestamp_ms = 1644465;

    GSControlStatus status;
    status.bits.RESERVED = 0;
    status.bits.state = 0;
    status.bits.isAllowDumpSwitchOn = 1;
    status.bits.isAllowFillSwitchOn = 1;
    status.bits.isArmIgniterSwitchOn = 0;
    status.bits.isArmServoSwitchOn = 1;
    status.bits.isEmergencyStopButtonPressed = 1;
    status.bits.isFireIgniterButtonPressed = 0;
    status.bits.isUnsafeKeySwitchPressed = 0;
    status.bits.isValveStartButtonPressed = 0;
    packet.fields.status = status;

    GSControlErrorStatus errorStatus;
    errorStatus.bits.invalidState = 0;
    errorStatus.bits.notInitialized = 0;
    errorStatus.bits.RESERVED = 0;
    packet.fields.errorStatus = errorStatus;

    packet.fields.lastReceivedGSCommandTimestamp_ms = 1643584;
    packet.fields.lastBoardSentCommandCode = BOARD_COMMAND_CODE_UNSAFE;
    packet.fields.lastSentCommandTimestamp_ms = 1644402;

    for (size_t i = 0; i < 16; i++) {
        packet.fields.padding[i] = 0;
    }

    const uint32_t expectedCRC = 2285882212u;
    const uint32_t computedCRC = CRC::computeCrc(packet.data, sizeof(packet) - sizeof(packet.fields.crc));
    CHECK(expectedCRC == computedCRC);
}

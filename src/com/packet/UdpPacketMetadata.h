#pragma once

#include "UDPPacket.h"
#include "UDPDeviceCtrlFlags.h"
#include <cstdint>

#include "framing/ethernet_header.hpp"

struct UdpPacketMetadata {
    //From original PacketMetadata
    enum class Status {NONE, VALID, INVALID, DUMP_IMMEDIATELY};

    Status status{Status::NONE};
    size_t size{};

    //UDP purposes
    std::uint32_t deviceID{};
    std::uint8_t  payloadType{}; // common-protocol PayloadType (Command / Telemetry / Response)
    std::uint32_t payloadID{};
    std::uint32_t deviceTsMs{};

    UDPDeviceCtrlFlags deviceCtrlFlags{}; // TODO: no equivalent in EthernetHeader; left default until GS-control telemetry returns
    std::uint8_t deviceState{};
    std::uint8_t seq{}; // EthernetHeader seq: a command's reply (Pong/Ack) echoes the command's seq


    static UdpPacketMetadata fromNetworkFrame(const EthernetHeader& rawFrame, size_t verifiedPayloadSize) {
        UdpPacketMetadata cleanData;

        cleanData.status = Status::VALID;
        cleanData.size = verifiedPayloadSize;

        cleanData.deviceID = rawFrame.sender_id;
        cleanData.payloadType = rawFrame.payload_type;
        cleanData.payloadID = rawFrame.payload_id;

        cleanData.deviceState = rawFrame.sender_state;
        cleanData.seq = rawFrame.seq;
        cleanData.deviceTsMs = rawFrame.sender_timestamp_ms; // native little-endian end-to-end in the new protocol; no ntohl
        return cleanData;
    }
};

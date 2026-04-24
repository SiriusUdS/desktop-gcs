#pragma once

#include <cstdint>
#include <array>
#include "UDPDeviceCtrlFlags.h"

namespace networking {

/**
 * UDP PACKET :
 * UDPPacketHeader (12 bytes)
 * Payload (length is a multiple of 4 bytes)
 * CRC (4 bytes)
 */

#pragma pack(push, 1)

struct FrameUDPPacketHeader {
    std::uint32_t deviceId      : 8;
    std::uint32_t payloadId     : 8;
    std::uint32_t payloadLength : 16;

    UDPDeviceCtrlFlags deviceCtrlFlags;
    std::uint8_t  deviceState;
    std::uint16_t reserved;
    std::uint32_t deviceTsMs;
};

union UDPPacketHeader {
    FrameUDPPacketHeader frame;
    std::array<std::uint8_t, sizeof(FrameUDPPacketHeader)> bytes;
    
    bool isValid() const {
        return frame.deviceId != 0;
    }
};

#pragma pack(pop)

static_assert(sizeof(FrameUDPPacketHeader) == 12, "UDPPacketHeader size must be 12 bytes");

} // namespace networking
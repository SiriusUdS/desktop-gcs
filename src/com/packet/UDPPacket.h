#pragma once
#include <array>
#include <cstdint>

#include "framing/ethernet_header.hpp"

namespace networking {

/**
 * UDP PACKET :
 * EthernetHeader (12 bytes)  -- common-protocol framing
 * Payload (length is a multiple of 4 bytes)
 * CRC (4 bytes)
 */

// Thin union over common-protocol's EthernetHeader so the receiver can fill the
// 12 header bytes incrementally, then read typed fields out of `frame`.
union UDPPacketHeader {
    EthernetHeader frame;
    std::array<std::uint8_t, sizeof(EthernetHeader)> bytes;

    bool isValid() const {
        return frame.sender_id != 0;
    }
};

} // namespace networking

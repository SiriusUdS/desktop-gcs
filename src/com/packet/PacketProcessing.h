#pragma once

#include "CRC.h"
#include "Logging.h"

#include <stdint.h>
#include <typeinfo>

namespace PacketProcessing {
void processIncomingPackets();

template <typename PacketType>
bool isPacketIntegrityValid(uint8_t* packetBuffer, PacketType packet, size_t sizeInBytes) {
    uint32_t computedCrc = CRC::computeCrc(packetBuffer, sizeInBytes - sizeof(packet->fields.crc));
    if (computedCrc != packet->fields.crc) {
        GCS_APP_LOG_WARN("PacketProcessing: CRC mismatch in {}, computed: {}, received: {}. Ignoring packet.",
                         typeid(*packet).name(),
                         computedCrc,
                         packet->fields.crc);
        return false;
    }
    return true;
}

} // namespace PacketProcessing

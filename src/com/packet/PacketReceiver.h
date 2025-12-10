#pragma once

#include "PacketCircularBuffer.h"
#include "PacketFramer.h"

/**
 * @class PacketReceiver
 * @brief Handles packet reception
 */
class PacketReceiver {
public:
    bool receiveByte(uint8_t byte);
    bool getPacket(uint8_t* recv);
    uint8_t* getBuffer();
    bool dumpNextPacket();
    void clear();
    bool packetAvailable() const;
    std::optional<PacketMetadata> nextPacketMetadata() const;

private:
    PacketCircularBuffer buf;
    PacketFramer pf{buf};
    std::queue<PacketMetadata> packetMetadataQueue{};
};

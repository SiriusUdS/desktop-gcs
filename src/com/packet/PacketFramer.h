#pragma once

#include "PacketCircularBuffer.h"
#include "PacketMetadata.h"

#include <queue>

/**
 * @class PacketFramer
 * @brief Frames packets by reading the bytes written into a circular buffer
 */
class PacketFramer {
public:
    PacketFramer(const PacketCircularBuffer& buf);
    bool tryFrame();
    void byteWritten();
    void newPacket();
    void clear();
    std::optional<PacketMetadata> getLastPacketMetadata() const;

private:
    bool checkForPacketStart();
    bool getHeaderFromBuf(size_t headerSize);

    static constexpr size_t MAX_HEADER_SIZE = 4;

    const PacketCircularBuffer& buf;
    uint8_t headerBuf[MAX_HEADER_SIZE];
    std::optional<PacketMetadata> lastPacketMetadata;
    PacketMetadata currentPacketMetadata;
    bool readingValidPacket{};
};

#pragma once
#include "CircularBuffer.hpp"
#include "CRC.h"
#include "PacketMetadata.h"
#include <queue>
#include <UdpPacket.h>
#include "UdpPacketMetadata.h"
#include <winsock2.h>

enum class ReceiverStatus {
    WAITING_FOR_HEADER,
    READING_PAYLOAD,
    READING_CRC
};

class UdpPacketReceiver {
public:
    UdpPacketReceiver();
    uint8_t* getBuffer();
    void receiveByte(uint8_t byte);
    bool getPacket(uint8_t* recv);
    std::optional<UdpPacketMetadata> nextPacketMetadata();
    bool packetAvailable();
    bool clear();
    uint8_t getAmountOfLostPackets();
private:
    ReceiverStatus status = ReceiverStatus::WAITING_FOR_HEADER;
    size_t currentByteCount = 0;
    uint8_t packetLostCount = 0;
    networking::UDPPacketHeader headerBuffer;
    uint32_t expectedPayloadLength = 0;
    uint8_t crcBuffer[4];
    
    std::queue<UdpPacketMetadata> packetMetadataQueue;
    CircularBuffer<4096> packetBuffer;
    std::vector<uint8_t> tempPayloadBuffer;
    
    void resetReceiverState();
    bool validateChecksum();
};

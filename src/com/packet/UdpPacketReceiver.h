#pragma once
#include "CircularBuffer.hpp"
#include "CRC.h"
#include "UdpConfig.h"

#include <queue>
#include <UdpPacket.h>
#include "UdpPacketMetadata.h"

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
    bool hasValidPayloadSize(uint32_t payloadLength);
    uint64_t getAmountOfLostPackets();
    uint64_t getAmountOfReceivedPackets();
private:
    ReceiverStatus status = ReceiverStatus::WAITING_FOR_HEADER;
    size_t currentByteCount = 0;
    uint64_t packetLostCount = 0;
    uint64_t totalPacketReceivedCount = 0; //Includes lost packets
    networking::UDPPacketHeader headerBuffer;
    uint32_t expectedPayloadLength = 0;
    uint8_t crcBuffer[UdpConfig::CRCSize];
    
    std::queue<UdpPacketMetadata> packetMetadataQueue;
    CircularBuffer<UdpConfig::UDPBufferSize> packetBuffer;
    std::vector<uint8_t> tempPayloadBuffer;
    
    void resetReceiverState();
    bool validateChecksum();
};

#pragma once
#include "PacketMetadata.h"
#include <queue>
#include <UdpPacket.h>

enum class ReceiverStatus {
    WAITING_FOR_HEADER,
    READING_PAYLOAD,
    READING_CRC
};

//TODO ADD Circular Buffer
class UdpPacketReciever {
public:
    UdpPacketReciever();
    
    void receiveByte(uint8_t byte);
    bool getPacket(uint8_t recv);
    uint8_t* getBuffer();
    void clear();
private:
    ReceiverStatus status = ReceiverStatus::WAITING_FOR_HEADER;
    size_t currentByteCount = 0;
    
    networking::UDPPacketHeader headerBuffer;
    uint32_t expectedPayloadLength = 0;
    uint8_t crcBuffer[4];
    
    std::queue<PacketMetadata> packetMetadataQueue;
    
    void resetReceiverState();
    bool validateChecksum();
};

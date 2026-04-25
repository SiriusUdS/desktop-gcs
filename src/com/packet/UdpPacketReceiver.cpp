#include "UdpPacketReceiver.h"


UdpPacketReceiver::UdpPacketReceiver() {
    resetReceiverState();
}

uint8_t* UdpPacketReceiver::getBuffer() {
    return packetBuffer.getBuffer();
}

void UdpPacketReceiver::resetReceiverState() {
    status = ReceiverStatus::WAITING_FOR_HEADER;
    currentByteCount = 0;
    expectedPayloadLength = 0;
}

void UdpPacketReceiver::receiveByte(uint8_t byte) {
    switch (status) {
    case ReceiverStatus::WAITING_FOR_HEADER: {
        headerBuffer.bytes[currentByteCount++] = byte;

        if (currentByteCount == sizeof(networking::UDPPacketHeader)) {
            expectedPayloadLength = ntohs(headerBuffer.frame.payloadLength); //Switch bcs Network use MSB
            
            tempPayloadBuffer.clear();
            tempPayloadBuffer.reserve(expectedPayloadLength);
            
            status = ReceiverStatus::READING_PAYLOAD;
            currentByteCount = 0;
        }
        break;
    }

    case ReceiverStatus::READING_PAYLOAD: {
        tempPayloadBuffer.push_back(byte);
        currentByteCount++;
        if (currentByteCount == expectedPayloadLength) {
            status = ReceiverStatus::READING_CRC;
            currentByteCount = 0;
        }
        break;
    }

    case ReceiverStatus::READING_CRC: {
        crcBuffer[currentByteCount++] = byte;

        if (currentByteCount == 4) {
            if (validateChecksum()) {
                for (uint8_t b:tempPayloadBuffer) {
                    packetBuffer.writeByte(b);
                }
                UdpPacketMetadata cleanPacket = UdpPacketMetadata::fromNetworkFrame(headerBuffer.frame, expectedPayloadLength);
                packetMetadataQueue.push(cleanPacket);
            } else {
                packetLostCount++;
            }
            resetReceiverState();
        }
        break;
    }
    }
}

bool UdpPacketReceiver::validateChecksum() {
    uint32_t receivedCRC = (crcBuffer[0] << 24) | (crcBuffer[1] << 16) | (crcBuffer[2] << 8) | crcBuffer[3];
    
    uint32_t calculatedCRC = CRC::computeCrc(tempPayloadBuffer.data(), expectedPayloadLength);
    
    return(receivedCRC == calculatedCRC);
}

bool UdpPacketReceiver::getPacket(uint8_t* recv) {
    if (packetMetadataQueue.empty()) {
        return false;
    }
    
    UdpPacketMetadata packetMetadata = packetMetadataQueue.front();
    
    packetMetadataQueue.pop();
    
    if (!packetBuffer.read(recv, packetMetadata.size)) {
        clear();
        return false;
    }
    
    return true;
}

std::optional<UdpPacketMetadata> UdpPacketReceiver::nextPacketMetadata() {
    if (packetMetadataQueue.empty()) {
        return std::nullopt;
    }
    
    return packetMetadataQueue.front();
}


bool UdpPacketReceiver::packetAvailable() {
    return !packetMetadataQueue.empty();
}

bool UdpPacketReceiver::clear() {
    packetBuffer.clear();
    while (!packetMetadataQueue.empty()) {
        packetMetadataQueue.pop();
    }
    resetReceiverState();
    return true;
}

uint8_t UdpPacketReceiver::getAmountOfLostPackets() {
    return packetLostCount;
}
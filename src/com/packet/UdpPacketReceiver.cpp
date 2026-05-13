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
        GCS_APP_LOG_DEBUG("WAITING_FOR_HEADER");
        headerBuffer.bytes[currentByteCount++] = byte;

        if (currentByteCount == sizeof(networking::UDPPacketHeader)) {
            expectedPayloadLength = ntohs(headerBuffer.frame.payloadLength); //Switch bcs Network use MSB

            tempPayloadBuffer.clear();
            tempPayloadBuffer.reserve(expectedPayloadLength);
            if (expectedPayloadLength == 0) {
                status = ReceiverStatus::READING_CRC;
            } else {
                status = ReceiverStatus::READING_PAYLOAD;
            }
            currentByteCount = 0;
        }

        if (!hasValidPayloadSize(expectedPayloadLength)) {
            status = ReceiverStatus::WAITING_FOR_HEADER;
            tempPayloadBuffer.clear();
            clear();
        }
        break;
    }

    case ReceiverStatus::READING_PAYLOAD: {
        GCS_APP_LOG_DEBUG("READING_PAYLOAD");
        tempPayloadBuffer.push_back(byte);
        currentByteCount++;
        if (currentByteCount == expectedPayloadLength) {
            status = ReceiverStatus::READING_CRC;
            currentByteCount = 0;
        }
        break;
    }

    case ReceiverStatus::READING_CRC: {
        GCS_APP_LOG_DEBUG("READING_CRC");
        crcBuffer[currentByteCount++] = byte;

        if (currentByteCount == 4) {
            if (validateChecksum()) {
                for (uint8_t b : tempPayloadBuffer) {
                    packetBuffer.writeByte(b);
                }
                UdpPacketMetadata cleanPacket = UdpPacketMetadata::fromNetworkFrame(headerBuffer.frame, expectedPayloadLength);
                packetMetadataQueue.push(cleanPacket);
            } else {
                packetLostCount++;
            }
            totalPacketReceivedCount++;
            resetReceiverState();
        }
        
        break;
    }
    }
}

bool UdpPacketReceiver::validateChecksum() {
    uint32_t receivedCRC = ntohl(std::bit_cast<uint32_t>(crcBuffer));
    uint32_t calculatedCRC = CRC::computeCrc(tempPayloadBuffer.data(), expectedPayloadLength);

    return (receivedCRC == calculatedCRC);
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

bool UdpPacketReceiver::hasValidPayloadSize(uint32_t payloadLength) {
    if (payloadLength > expectedPayloadLength) {
        return false;
    }

    if (payloadLength % 4 != 0) {
        return false;
    }

    return true;
}

uint64_t UdpPacketReceiver::getAmountOfLostPackets() {
    return packetLostCount;
}

uint64_t UdpPacketReceiver::getAmountOfReceivedPackets() {
    return totalPacketReceivedCount;
}
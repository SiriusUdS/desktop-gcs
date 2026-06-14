#include "UdpPacketReceiver.h"

#include "PacketProcessing.h"
#include "UdpCom.h"


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

void UdpPacketReceiver::receiveByte(uint8_t byte, bool isTesting = false) {
    switch (status) {
    case ReceiverStatus::WAITING_FOR_HEADER: {
        headerBuffer.bytes[currentByteCount++] = byte;

        if (currentByteCount == sizeof(networking::UDPPacketHeader)) {
            expectedPayloadLength = headerBuffer.frame.payload_size_bytes;

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
                for (uint8_t b : tempPayloadBuffer) {
                    packetBuffer.writeByte(b);
                }
                UdpPacketMetadata cleanPacket = UdpPacketMetadata::fromNetworkFrame(headerBuffer.frame, expectedPayloadLength);
                packetMetadataQueue.push(cleanPacket);
                if (!isTesting) {
                    PacketProcessing::processIncomingPackets(); //Very high data rate, need to ve called here, if in test we do not want to process there to allow for checks
                }
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
    uint32_t receivedCRC = (std::bit_cast<uint32_t>(crcBuffer));

    // The CRC covers our EthernetHeader + payload (the UDP/IP transport header has
    // its own checksum and is excluded). Mirrors the GCS command-frame CRC so both
    // directions agree.
    std::vector<uint8_t> crcInput;
    crcInput.reserve(headerBuffer.bytes.size() + tempPayloadBuffer.size());
    crcInput.insert(crcInput.end(), headerBuffer.bytes.begin(), headerBuffer.bytes.end());
    crcInput.insert(crcInput.end(), tempPayloadBuffer.begin(), tempPayloadBuffer.end());

    uint32_t calculatedCRC = CRC::computeCrc32(crcInput.data(), crcInput.size());

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
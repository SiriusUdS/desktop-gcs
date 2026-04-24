#include "UdpPacketReciever.h"
#include <winsock2.h>

UdpPacketReciever::UdpPacketReciever() {
    resetReceiverState();
}

void UdpPacketReciever::resetReceiverState() {
    status = ReceiverStatus::WAITING_FOR_HEADER;
    currentByteCount = 0;
    expectedPayloadLength= 0;
}

void UdpPacketReciever::receiveByte(uint8_t byte) {
    switch (status) {
        case ReceiverStatus::WAITING_FOR_HEADER: {
            headerBuffer.bytes[currentByteCount++] = byte;
            
            if (currentByteCount == sizeof(networking::FrameUDPPacketHeader)) {
                expectedPayloadLength =  ntohl(headerBuffer.frame.payloadLength); //Switch bcs Network use MSB
                status = ReceiverStatus::READING_PAYLOAD;
                currentByteCount = 0;
            }
            break;
        }
            
        
        case ReceiverStatus::READING_PAYLOAD: {
            //TODO SEND TO CRC BUFFER
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
                    PacketMetadata metadata;
                    metadata.size = expectedPayloadLength;
                    //TODO add other fields from headerBuffer
                    packetMetadataQueue.push(metadata);
                } else {
                    //TODO accumulate packet loss
                }
                resetReceiverState();
            }
            break;
        }
        
    }
    
}
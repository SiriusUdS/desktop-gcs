#include "doctest.h"
#include "UDPPacket.h"
#include "UdpPacketMetadata.h"

#include <winsock2.h>

TEST_CASE("UdpPacketMetadata should correctly parse network endianness") {
    EthernetHeader rawFrame;

    for (int i = 0; i < sizeof(EthernetHeader); i++) {
        ((uint8_t*)&rawFrame)[i] = 0;
    }

    rawFrame.sender_id = 1;
    rawFrame.payload_id = 1;

    uint32_t expectedTime = 987654321;
    rawFrame.sender_timestamp_ms = expectedTime; // native little-endian end-to-end; no byte swap
    
    UdpPacketMetadata meta = UdpPacketMetadata::fromNetworkFrame(rawFrame, 16);
    
    CHECK(meta.status == UdpPacketMetadata::Status::VALID);
    CHECK(meta.size == 16);
    CHECK(meta.deviceID == 1);
    CHECK(meta.payloadID == 1);
    
    CHECK(meta.deviceTsMs == expectedTime);
}
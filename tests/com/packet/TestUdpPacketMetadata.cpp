#include "doctest.h"
#include "UDPPacket.h"
#include "UdpPacketMetadata.h"

#include <winsock2.h>

TEST_CASE("UdpPacketMetadata should correctly parse network endianness") {
    networking::FrameUDPPacketHeader rawFrame;
    
    for (int i = 0; i < sizeof(networking::FrameUDPPacketHeader); i++) {
        ((uint8_t*)&rawFrame)[i] = 0;
    }
    
    rawFrame.deviceId = 1;
    rawFrame.payloadId = 1;
    
    uint32_t expectedTime = 987654321;
    rawFrame.deviceTsMs = htonl(expectedTime);
    
    UdpPacketMetadata meta = UdpPacketMetadata::fromNetworkFrame(rawFrame, 16);
    
    CHECK(meta.status == UdpPacketMetadata::Status::VALID);
    CHECK(meta.size == 16);
    CHECK(meta.deviceID == 1);
    CHECK(meta.payloadID == 1);
    
    CHECK(meta.deviceTsMs == expectedTime);
}
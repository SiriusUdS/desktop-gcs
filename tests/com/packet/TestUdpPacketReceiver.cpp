#include "CRC.h"
#include "doctest.h"
#include "UdpPacketReceiver.h"

void pushUdpPacket(UdpPacketReceiver& pr, uint8_t deviceId, uint8_t payloadId, std::vector<uint8_t> payload, bool corruptCrc = false) {
    networking::UDPPacketHeader header;
    
    for (int i = 0; i < sizeof(networking::UDPPacketHeader); i++) {
        header.bytes[i] = 0;
    }
    
    header.frame.deviceId = deviceId;
    header.frame.payloadId = payloadId;
    
    header.frame.payloadLength = htons(static_cast<uint16_t>(payload.size()));
    
    header.frame.deviceCtrlFlags.flags = 0;
    header.frame.deviceState = 0;
    header.frame.reserved = 0;
    header.frame.deviceTsMs = htonl(12345);
    
    //Header
    for (size_t i = 0; i < sizeof(networking::UDPPacketHeader); i++) {
        pr.receiveByte(header.bytes[i]);
    }
    
    //Payload
    for (uint8_t b: payload) {
        pr.receiveByte(b);
    }
    
    //CRC
    uint32_t crc = CRC::computeCrc(const_cast<uint8_t*>(payload.data()), payload.size());
    if (corruptCrc) {
        crc ^= 0xFFFFFFFF; //Inverts bits
    }
    
    pr.receiveByte((crc >> 24) & 0xFF);
    pr.receiveByte((crc >> 16) & 0xFF);
    pr.receiveByte((crc >> 8) & 0xFF);
    pr.receiveByte(crc & 0xFF);
}

TEST_CASE("UdpPacketReceiver should successfully receive and validate a correct packet") {
    UdpPacketReceiver pr;
    std::vector<uint8_t> payload = {0xDE, 0xAD, 0xBE, 0xEF};
    
    CHECK_FALSE(pr.packetAvailable());
    
    pushUdpPacket(pr, 0x01, 0x02, payload);
    
    CHECK(pr.packetAvailable());
    
    auto meta = pr.nextPacketMetadata();
    
    REQUIRE(meta.has_value());
    CHECK(meta->status == UdpPacketMetadata::Status::VALID);
    CHECK(meta->size == payload.size());
    CHECK(meta->deviceID == 0x01);
    CHECK(meta->payloadID == 0x02);
    CHECK(meta->deviceTsMs == 12345);
    
    uint8_t buffer[10] = {0};
    CHECK(pr.getPacket(buffer));
    CHECK(buffer[0] == 0xDE);
    CHECK(buffer[1] == 0xAD);
    CHECK(buffer[2] == 0xBE);   
    CHECK(buffer[3] == 0xEF);
    
    CHECK_FALSE(pr.packetAvailable());
}

TEST_CASE("UdpPacketReciever should reject packet with invalid CRC") {
    UdpPacketReceiver pr;
    std::vector<uint8_t> payload = {0x01, 0x02, 0x03, 0x04};
    
    pushUdpPacket(pr, 0x01, 0x02, payload, true);
    
    CHECK_FALSE(pr.packetAvailable());
    CHECK_FALSE(pr.nextPacketMetadata().has_value());
}

TEST_CASE("UdpPacketReceiver should queue and handle multiple packets sequentially") {
    UdpPacketReceiver pr;
    std::vector<uint8_t> payload1 = {0xAA, 0xBB, 0xCC, 0xDD};
    std::vector<uint8_t> payload2 = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    
    pushUdpPacket(pr, 10, 20, payload1);
    pushUdpPacket(pr, 30, 40, payload2);
    
    CHECK(pr.packetAvailable());
    
    //Check first packet
    auto meta1 = pr.nextPacketMetadata();
    REQUIRE(meta1.has_value());
    CHECK(meta1->size == 4);
    CHECK(meta1->deviceID == 10);
    
    uint8_t buf1[4] = {0};
    CHECK(pr.getPacket(buf1));
    CHECK(buf1[0] == 0xAA);
    CHECK(buf1[1] == 0xBB);
    CHECK(buf1[2] == 0xCC);
    CHECK(buf1[3] == 0xDD);
    
    //Check second packet
    auto meta2 = pr.nextPacketMetadata();
    REQUIRE(meta2.has_value());
    CHECK(meta2->size == 8);
    CHECK(meta2->deviceID == 30);
    
    uint8_t buf2[8] = {0};
    CHECK(pr.getPacket(buf2));
    CHECK(buf2[0] == 0x11);
    CHECK(buf2[1] == 0x22);
    CHECK(buf2[2] == 0x33);
    CHECK(buf2[3] == 0x44);
    CHECK(buf2[4] == 0x55);
    CHECK(buf2[5] == 0x66);
    CHECK(buf2[6] == 0x77);
    CHECK(buf2[7] == 0x88);
    
    CHECK_FALSE(pr.packetAvailable());
}

TEST_CASE("UdpPacketReceiver should clear internal state correctly") {
    UdpPacketReceiver pr;
    std::vector<uint8_t> payload = {0x11, 0x22, 0x33, 0x44};
    
    pushUdpPacket(pr, 1, 1, payload);
    CHECK(pr.packetAvailable());
    
    pr.clear();
    
    CHECK_FALSE(pr.packetAvailable());
    CHECK_FALSE(pr.nextPacketMetadata().has_value());
    
    pushUdpPacket(pr, 2, 2, payload);
    CHECK(pr.packetAvailable());
}

TEST_CASE("UdpPacketReceiver track lost packets") {
    UdpPacketReceiver pr;
    std::vector<uint8_t> payload = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    pushUdpPacket(pr, 1, 1, payload, true);
    
    CHECK_FALSE(pr.packetAvailable());
    CHECK_FALSE(pr.nextPacketMetadata().has_value());
    
    CHECK(pr.getAmountOfLostPackets() == 1);
}
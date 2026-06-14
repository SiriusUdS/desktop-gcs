#include "doctest.h"
#include "PacketProcessing.h"
#include "UdpCom.h"

TEST_CASE("UdpCom should successfully bind, read and process a UDP packet from localhost") {
    UdpCom com;
    com.start();
    com.setIsTest(true);
    
    REQUIRE(com.comOpened());
    REQUIRE(com.getConnectionDetails().has_value());
    
    sockpp::udp_socket client;
    
    std::vector<uint8_t> payload = {0xAA, 0xBB, 0xCC, 0xDD};
    std::vector<uint8_t> networkBuffer;
    
    networking::UDPPacketHeader header;
    for (int i = 0; i < sizeof(networking::UDPPacketHeader); i++) {
        header.bytes[i] = 0;
    }
    
    header.frame.sender_id = 1;
    header.frame.payload_id = 2;
    header.frame.payload_size_bytes = (static_cast<uint16_t>(payload.size()));
    header.frame.sender_timestamp_ms = (12345);
    
    for (size_t i = 0; i < sizeof(header); i++) {
        networkBuffer.push_back(header.bytes[i]);
    }
    
    for (uint8_t b:payload) {
        networkBuffer.push_back(b);
    }
    
    // CRC over header + payload (networkBuffer currently holds exactly those bytes)
    uint32_t crc = CRC::computeCrcUDP(networkBuffer.data(), networkBuffer.size());
    networkBuffer.push_back(crc & 0xFF);
    networkBuffer.push_back((crc>>8) & 0xFF);
    networkBuffer.push_back((crc>>16) & 0xFF);
    networkBuffer.push_back((crc>>24) & 0xFF);
    
    ssize_t bytesSent = client.send_to(networkBuffer.data(), networkBuffer.size(), sockpp::inet_address("127.0.0.1", UdpConfig::defaultReceivePort));
    REQUIRE(bytesSent == networkBuffer.size());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    bool readSuccess = com.read();
    CHECK(readSuccess == true);
    
    uint8_t recvBuf[256] = {0};
    bool gotPacket = com.getPacket(recvBuf);
    REQUIRE(gotPacket == true);
    
    CHECK(recvBuf[0] == 0xAA);
    CHECK(recvBuf[1] == 0xBB);
    CHECK(recvBuf[2] == 0xCC);
    CHECK(recvBuf[3] == 0xDD);
    
    com.shutdown();
    CHECK_FALSE(com.comOpened());
}

TEST_CASE("UdpCom should write data to the destination address") {
    UdpCom com;
    com.start();
    com.setIsTest(true);
    REQUIRE(com.comOpened());
    
    sockpp::udp_socket receiver;
    receiver.bind(sockpp::inet_address("0.0.0.0", UdpConfig::defaultDestPort));
    
    std::vector<uint8_t> outgoingMessage = {0x01, 0x02, 0x03, 0x04};
    std::span<const uint8_t> msgSpan(outgoingMessage);
    
    bool writeSuccess = com.write(msgSpan);
    CHECK(writeSuccess == true);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    char incomingBuffer[128];
    sockpp::inet_address senderAddr;
    ssize_t bytesReceived = receiver.recv_from(incomingBuffer, sizeof(incomingBuffer), &senderAddr);
    
    REQUIRE(bytesReceived == 4);
    CHECK(incomingBuffer[0] == 0x01);
    CHECK(incomingBuffer[1] == 0x02);
    CHECK(incomingBuffer[2] == 0x03);
    CHECK(incomingBuffer[3] == 0x04);
    
    com.shutdown();
    
}
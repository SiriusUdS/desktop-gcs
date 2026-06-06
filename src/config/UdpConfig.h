#pragma once

namespace UdpConfig {
    constexpr const char* defaultDestIp = "127.0.0.1";
    constexpr uint16_t defaultDestPort = 5555;
    constexpr uint16_t defaultReceivePort = 7520;
    constexpr size_t UDPPacketHeaderSize = 12;
    constexpr size_t CRCSize = 4;
    constexpr size_t UDPBufferSize = 4096;
} //namespace UdpConfig
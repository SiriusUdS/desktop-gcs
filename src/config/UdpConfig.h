#pragma once

namespace UdpConfig {
    constexpr const char* defaultDestIp = "192.168.0.100"; // board (FCU) — destination for outbound commands
    constexpr uint16_t defaultDestPort = 55555; // FCU command-listen port
    constexpr uint16_t defaultReceivePort = 7520;
    constexpr size_t UDPPacketHeaderSize = 12;
    constexpr size_t CRCSize = 4;
    constexpr size_t UDPBufferSize = 4096;
} //namespace UdpConfig
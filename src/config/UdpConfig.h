#pragma once

namespace UdpConfig {
    constexpr const char* defaultDestIp = "192.168.0.101"; // board — destination for outbound commands
    constexpr uint16_t defaultDestPort = 7522; // board command-listen port
    constexpr uint16_t defaultReceivePort = 7520; // GS listen port (board sends telemetry here)
    constexpr size_t UDPPacketHeaderSize = 12;
    constexpr size_t CRCSize = 4;
    constexpr size_t UDPBufferSize = 4096;
} //namespace UdpConfig
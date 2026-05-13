#pragma once

#include "BoardComStateMonitor.h"
#include "../ComTask.h"
#include <cstdint>
#include "ICom.h"
#include <sockpp/inet_address.h>
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"
#include <sockpp/socket.h>
#include <string>
#include "Timer.h"
#include "UdpPacketReceiver.h"
#include <sockpp/udp_socket.h>


/**
 * @class UdpCom
 * @brief Handles UDP communication to receive packets
 */
class UdpCom: public ICom {
public:
    void start() override;
    bool read() override;
    bool write(std::span<const uint8_t> msg) override;
    bool openConnectionTo(std::string ipAddress, uint16_t sendPort, uint16_t receivePort);
    bool comOpened() override;
    void shutdown() override;
    bool getPacket(uint8_t* recv) override;
    uint8_t* getBuffer() override;
    std::string getProtocolName() override;
    std::optional<std::string> getConnectionDetails() override;
    ComType getComType() const override;
    

private:
    static constexpr int incomingDataBufferSize = UdpConfig::UDPBufferSize;
    ComType comType = ComType::UDP;
    uint16_t destPort = UdpConfig::defaultDestPort;
    std::string destIp = UdpConfig::defaultDestIp;
    uint16_t receivePort = UdpConfig::defaultReceivePort;
    

    sockpp::udp_socket sock;
    sockpp::inet_address destAddr;
    bool initialized = false;
};

#pragma once

#include "BoardComStateMonitor.h"
#include "../ComTask.h"
#include <cstdint>
#include "ICom.h"
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"
#include <string>
#include "Timer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

/**
 * @class UdpCom
 * @brief Handles UDP communication to receive packets
 */
class UdpCom: public ICom {
public:
    void start() override;
    bool read() override;
    bool write(uint8_t* msg, size_t size) override;
    bool comOpened() override;
    void shutdown() override;
    bool getPacket(uint8_t* recv) override;
    uint8_t* getBuffer() override;
    std::string getProtocolName() override;
    std::string getConnectionDetails() override;
    ComType getComType() const override;
    

private:
    ComType comType = ComType::UDP;
    int destPort = 5002;
    int receivePort = 5555;
    std::string destIp = "127.0.0.1";
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in destAddr;
    bool initialized = false;
};

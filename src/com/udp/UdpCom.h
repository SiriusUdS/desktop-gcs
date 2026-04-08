#pragma once

#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include "../serial/SerialTask.h"
#include "BoardComStateMonitor.h"
#include "ComPortSelector.h"
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"
/**
 * @class UdpCom
 * @brief Handles UDP communication to receive packets
 */
class UdpCom {
public:
    void start();
    bool read();
    bool write(uint8_t* msg, size_t size);
    bool comOpened();
    void shutdown();

    //TODO temp
    bool getPacket(uint8_t* recv);
    uint8_t* getBuffer();
    

private:
    int destPort = 5000;
    int receivePort = 5000;
    std::string destIp = "127.0.0.1";
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in destAddr;
    bool initialized = false;
};

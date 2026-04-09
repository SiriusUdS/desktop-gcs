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
 * @class ICom
 * @brief Communication Interface
 */
class ICom {
public:
    virtual void start() = 0;
    virtual bool read() = 0;
    virtual bool write(uint8_t* msg, size_t size) = 0;
    virtual bool comOpened() = 0;
    virtual void shutdown() = 0;
    virtual bool getPacket(uint8_t* recv) = 0;
    virtual uint8_t* getBuffer() = 0;
    virtual ~ICom() = default;
};

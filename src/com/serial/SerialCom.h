#pragma once

#include <ceserial.h>
#include "ICom.h"
#include "ComPortSelector.h"
#include "WindowsComPortDiscovery.h"
#include "Timer.h"

/**
 * @class SerialCom
 * @brief Handles serial communication to receive packets
 */
class SerialCom: public ICom {
public:
    void start() override;
    bool read() override;
    bool write(uint8_t* msg, size_t size) override;
    bool comOpened() const override;
    bool getPacket(uint8_t* recv) override;
    uint8_t* getBuffer() override;
    void shutdown() override;
    const char* getProtocolName() const override;
    const char* getConnectionDetails() const override;

private:
    ceSerial com;
    Timer timerSerialRead;
    WindowsComPortDiscovery comPortDiscovery;
    ComPortSelector comPortSelector{comPortDiscovery};
};

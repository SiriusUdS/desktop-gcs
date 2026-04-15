#pragma once

#include <ceserial.h>
#include "ComPortSelector.h"
#include "ICom.h"
#include <span>
#include "Timer.h"
#include "WindowsComPortDiscovery.h"

/**
 * @class SerialCom
 * @brief Handles serial communication to receive packets
 */
class SerialCom: public ICom {
public:
    SerialCom();
    void start() override;
    bool read() override;
    bool write(std::span<const uint8_t> msg) override;
    bool comOpened() override;
    bool getPacket(uint8_t* recv) override;
    uint8_t* getBuffer() override;
    void shutdown() override;
    std::string getProtocolName() override;
    std::optional<std::string> getConnectionDetails() override;
    ComType getComType() const override;

private:
    ComType comType = ComType::SERIAL;
    ceSerial com;
    Timer timerSerialRead;
    WindowsComPortDiscovery comPortDiscovery;
    ComPortSelector comPortSelector;
};

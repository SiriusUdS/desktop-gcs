#pragma once

#include <ceserial.h>
#include "ICom.h"
/**
 * @class SerialCom
 * @brief Handles serial communication to receive packets
 */
class SerialCom: public ICom {
public:
    void start() override;
    bool read() override;
    bool write(uint8_t* msg, size_t size) override;
    bool comOpened() override;
    bool getPacket(uint8_t* recv) override;
    uint8_t* getBuffer() override;
    void shutdown() override;

private:
    ceSerial com;
};

#pragma once

#include <ceserial.h>

/**
 * @class SerialCom
 * @brief Handles serial communication to receive packets
 */
class SerialCom {
public:
    void start();
    bool read();
    bool write(uint8_t* msg, size_t size);
    bool comOpened();
    bool getPacket(uint8_t* recv);
    uint8_t* getBuffer();
    void shutdown();

private:
    ceSerial com;
};

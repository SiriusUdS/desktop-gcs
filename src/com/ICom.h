#pragma once

#include <cstdint>

/**
 * @class ICom
 * @brief Communication Interface
 */
class ICom {
public:
    virtual void start() = 0;
    virtual bool read() = 0;
    virtual bool write(uint8_t* msg, size_t size) = 0;
    virtual bool comOpened() const = 0;
    virtual void shutdown() = 0;
    virtual bool getPacket(uint8_t* recv) = 0;
    virtual uint8_t* getBuffer() = 0;
    virtual ~ICom() = default;
    virtual const char* getProtocolName() const = 0;
    virtual const char* getConnectionDetails() const = 0;
};

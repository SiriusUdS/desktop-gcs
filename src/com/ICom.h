#pragma once

#include "ComType.h"
#include <cstdint>
#include <string>


/**
 * @class ICom
 * @brief Communication Interface
 */
class ICom {
public:
    virtual std::string protocolNameFromEnum(ComType comType);
    virtual void start() = 0;
    virtual bool read() = 0;
    virtual bool write(uint8_t* msg, size_t size) = 0;
    virtual bool comOpened() = 0;
    virtual void shutdown() = 0;
    virtual bool getPacket(uint8_t* recv) = 0;
    virtual uint8_t* getBuffer() = 0;
    virtual ~ICom() = default;
    virtual ComType getComType() const = 0;
    virtual std::string getProtocolName() = 0;
    virtual std::string getConnectionDetails() = 0;
};


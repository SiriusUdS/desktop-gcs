#pragma once

#include "ComType.h"
#include <cstdint>
#include <optional>
#include <span>
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
    virtual bool write(std::span<const uint8_t>) = 0;
    // Send a frame to an explicit destination instead of the configured one. Default no-op so
    // transports that don't support ad-hoc addressing (e.g. serial) need not implement it.
    virtual bool writeTo(std::span<const uint8_t>, const std::string& /*ip*/, uint16_t /*port*/) { return false; }
    virtual bool comOpened() = 0;
    virtual void shutdown() = 0;
    virtual bool getPacket(uint8_t* recv) = 0;
    virtual uint8_t* getBuffer() = 0;
    virtual ~ICom() = default;
    virtual ComType getComType() const = 0;
    virtual std::string getProtocolName() = 0;
    virtual std::optional<std::string> getConnectionDetails() = 0;
};


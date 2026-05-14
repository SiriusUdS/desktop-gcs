#pragma once

#include "CRC/CRCConstants.h"

#include <cstdint>

namespace CRC {
/**
 * @brief Computes the CRC32 checksum for a given data buffer.
 * It is assumed that the data buffer is aligned to 4-byte boundaries.
 *
 * @param data Pointer to the data buffer.
 * @param sizeInBytes Size of the data buffer in bytes.
 * @return The computed CRC32 checksum.
 */
inline std::uint32_t computeCrc(uint8_t* data, size_t sizeInBytes) {
    std::uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < sizeInBytes; i++) {
        // XOR the top byte of the CRC with the current data byte
        crc ^= (static_cast<uint32_t>(data[i]) << 24);
        
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80000000) {
                constexpr uint32_t POLY = CRC_GENERATING_POLYNOMIAL;
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}
} // namespace CRC

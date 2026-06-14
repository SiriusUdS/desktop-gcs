#pragma once

#include "system/crc32_polynomial.hpp"

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
inline std::uint32_t computeCrcUDP(uint8_t* data, size_t sizeInBytes) {
    std::uint32_t crc = 0xFFFFFFFF;
    
    for (size_t i = 0; i < sizeInBytes; i++) {
        // XOR the top byte of the CRC with the current data byte
        crc ^= (static_cast<uint32_t>(data[i]) << 24);
        
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80000000) {
                constexpr uint32_t POLY = logic::data_integrity::CRC32_POLYNOMIAL;
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

inline std::uint32_t computeCrcSerial(uint8_t* data, size_t sizeInBytes) {
    std::uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < sizeInBytes; i += 4) {
        const uint32_t word = *reinterpret_cast<uint32_t*>(&data[i]);

        crc ^= word;
        for (int bit = 0; bit < 32; bit++) {
            if (crc & 0x80000000) {
                constexpr uint32_t POLY = logic::data_integrity::CRC32_POLYNOMIAL;
                crc = (crc << 1) ^ POLY;
                continue;
            }
            crc <<= 1;
        }
    }
    return crc;
}

/**
 * @brief Standard CRC-32 (zlib / PNG / PKZIP): reflected input and output,
 * init 0xFFFFFFFF, final XOR 0xFFFFFFFF, reflected polynomial. This is the
 * common-protocol wire CRC, matching the boards' firmware.
 */
inline std::uint32_t computeCrc32(const uint8_t* data, size_t sizeInBytes) {
    std::uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < sizeInBytes; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc & 1u) ? ((crc >> 1) ^ logic::data_integrity::CRC32_POLYNOMIAL_REFLECTED) : (crc >> 1);
        }
    }
    return crc ^ 0xFFFFFFFFu;
}

} // namespace CRC

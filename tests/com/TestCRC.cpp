#include "CRC.h"

#include <doctest.h>

#include <array>
#include <cstdint>

// CRC regression tests over a fixed byte buffer, decoupled from any protocol
// struct. Reference values match the CRC.h implementation: init 0xFFFFFFFF,
// polynomial 0x04C11DB7, MSB-first, no final reflection or XOR-out.
namespace {
constexpr std::array<uint8_t, 16> kBuffer = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
}

TEST_CASE("computeCrcUDP returns the expected checksum for a known buffer") {
    std::array<uint8_t, 16> data = kBuffer;
    CHECK(CRC::computeCrcUDP(data.data(), data.size()) == 0xA97AFF4Du);
}

TEST_CASE("computeCrcSerial returns the expected checksum for a known buffer") {
    // computeCrcSerial reads the buffer 4 bytes at a time, so the size must be a
    // multiple of 4.
    std::array<uint8_t, 16> data = kBuffer;
    CHECK(CRC::computeCrcSerial(data.data(), data.size()) == 0x081B46CAu);
}

TEST_CASE("computeCrc32 matches the canonical CRC-32 check value") {
    // Standard CRC-32 (zlib / PNG): CRC32("123456789") == 0xCBF43926. This is the
    // common-protocol wire CRC, so matching it confirms the GCS agrees with the
    // boards' firmware.
    const char* check = "123456789";
    CHECK(CRC::computeCrc32(reinterpret_cast<const uint8_t*>(check), 9) == 0xCBF43926u);
}

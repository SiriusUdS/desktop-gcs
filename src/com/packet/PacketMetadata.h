#pragma once

/**
 * @struct PacketMetadata
 * @brief Metadata of received packets.
 */
struct PacketMetadata {
    /**
     * @enum Status
     * @brief Status of the packet.
     */
    enum class Status { NONE, VALID, INVALID, DUMP_IMMEDIATLY };
    Status status{Status::NONE}; ///< Status of the packet.
    size_t size{};               ///< Size of the packet in bytes.
};

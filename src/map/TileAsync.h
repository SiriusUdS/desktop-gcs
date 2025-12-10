#pragma once

#include <chrono>
#include <future>
#include <memory>

class ITile;
using namespace std::chrono_literals;

/**
 * @struct TileAsync
 * @brief Tile that gets its data asynchronously fetched
 */
struct TileAsync {
    /**
     * @struct FutureData
     * @brief Data that will be fetched in the future
     */
    struct FutureData {
        std::shared_ptr<ITile> tile; ///< Tile data
    };

    int z{};                        ///< Zoom level of the tile to be fetched
    int x{};                        ///< X position of the tile to be fetched
    int y{};                        ///< Y position of the tile to be fetched
    std::future<FutureData> future; ///< The tile data that will be fetched

    /**
     * @brief Checks if the tile data has been fetched
     * @returns True if the tile data is ready, else false
     */
    bool ready() const {
        return future.wait_for(0s) == std::future_status::ready;
    }

    /**
     * @brief Checks if two tiles are equal (same x, y, z)
     * @param other The tile to which the current tile is compared to
     * @returns True if both tiles are equal, else false
     */
    bool operator==(const TileAsync& other) const {
        return this->z == other.z && this->x == other.x && this->y == other.y;
    }
};

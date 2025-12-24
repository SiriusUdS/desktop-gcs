#pragma once

#include <atomic>
#include <stdint.h>

/**
 * @struct ValveData
 * @brief Valve data received from the boards.
 */
struct ValveData {
    std::atomic<bool> isIdle{};                 ///< Valve is idle.
    std::atomic<bool> closedSwitchHigh{};       ///< Closed switch is high.
    std::atomic<bool> openedSwitchHigh{};       ///< Opened switch is high.
    std::atomic<uint16_t> positionOpened_pct{}; ///< Position opened in percentage.
};

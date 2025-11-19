#ifndef VALVEDATA_H
#define VALVEDATA_H

#include <stdint.h>

/**
 * @struct ValveData
 * @brief Valve data received from the boards.
 */
struct ValveData {
    bool isIdle{};                 ///< Valve is idle.
    bool closedSwitchHigh{};       ///< Closed switch is high.
    bool openedSwitchHigh{};       ///< Opened switch is high.
    uint16_t positionOpened_pct{}; ///< Position opened in percentage.
};

#endif // VALVEDATA_H

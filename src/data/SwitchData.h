#ifndef SWITCHDATA_H
#define SWITCHDATA_H

#include <atomic>

/**
 * @struct SwitchData
 * @brief Represents the state of a switch/button
 */
struct SwitchData {
    std::atomic<bool> isOn{}; ///< Whether the switch is on or off
};

#endif // SWITCHDATA_H

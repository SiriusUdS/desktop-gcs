#pragma once

#include "Timer.h"

/**
 * @class BoardComStateMonitor
 * @brief Monitors the state of the serial communication with a specific board.
 */
class BoardComStateMonitor {
public:
    /**
     * @enum State
     * @brief Represents the state of the serial communication with a specific board.
     */
    enum State {
        STARTING,    ///< Communication is starting
        WORKING,     ///< Communication is working
        NOT_WORKING, ///< Communication is not working
    };

    void trackSuccessfulPacketRead();
    void reset();
    State getState() const;

    // clang-format off
    static constexpr size_t TIME_WITHOUT_SUCCESSFUL_PACKET_READ_BEFORE_FAILURE_SEC = 3; ///< Amount of time without packet received before considering communication as failing.
    static constexpr size_t TIME_WITHOUT_INITIAL_PACKET_READ_BEFORE_FAILURE_SEC = 5; ///< Amount of time without first packet received since start before considering communication as failing.
    // clang-format on

private:
    bool ioSuccessSinceStart{};
    Timer lastSuccessfulPacketReadTimer;
};

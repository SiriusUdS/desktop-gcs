#pragma once

#include <chrono>

/**
 * @class IntervalTimer
 * @brief A timer that counts the number of elapsed intervals.
 */
class IntervalTimer {
public:
    IntervalTimer(std::chrono::steady_clock::duration interval);
    void waitUntilNextInterval() const;

private:
    std::chrono::steady_clock::duration getTimeUntilNextInterval() const;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::duration interval;
};

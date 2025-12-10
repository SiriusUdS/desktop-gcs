#pragma once

#include <chrono>

/**
 * @class Timer
 * @brief A simple timer class that measures elapsed time.
 */
class Timer {
public:
    Timer();
    Timer(std::chrono::steady_clock::duration duration);
    double getElapsedTimeInSeconds() const;
    bool hasElapsed() const;
    void reset();

private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::duration duration{};
};

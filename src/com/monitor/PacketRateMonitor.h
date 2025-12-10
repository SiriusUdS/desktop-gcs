#pragma once

#include <chrono>
#include <deque>
#include <mutex>

/**
 * @class PacketRateMonitor
 * @brief Monitors the rate of incoming packets over a specified time window.
 */
class PacketRateMonitor {
public:
    void trackPacket();
    double getRatePerSecond();
    void reset();

    static constexpr double TIME_WINDOW_SECONDS = 5.0; ///< Time window in seconds for calculating the packet rate.

private:
    void removeOldTimePoints();

    std::deque<std::chrono::steady_clock::time_point> timePoints;
    std::mutex mtx;
};

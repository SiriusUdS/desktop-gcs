#pragma once

#include "GSDataCenterConfig.h"

#include <array>
#include <cstdint>
#include <mutex>

/**
 * @class AdcChannelAverager
 * @brief Thread-safe down-sampler that averages the high-rate (~2 kHz) ADC channel
 * samples in fixed windows of WINDOW samples, publishing one averaged set per
 * window (~125 Hz at 2 kHz). The decode submits each SystemState record's raw
 * channels (producer thread); the UI reads the latest published average at its own
 * cadence, so the rendered values reflect every sample received rather than a
 * single sub-sampled reading.
 */
class AdcChannelAverager {
public:
    static constexpr size_t CHANNELS = GSDataCenterConfig::ADC_CHANNEL_AMOUNT;
    static constexpr uint32_t WINDOW_SHIFT = 4;           // log2(WINDOW)
    static constexpr uint32_t WINDOW = 1u << WINDOW_SHIFT; // 16 samples per average (~125 Hz at 2 kHz)

    void submit(const int32_t* channels) {
        std::lock_guard<std::mutex> lock(mtx);
        for (size_t i = 0; i < CHANNELS; i++) {
            sum[i] += channels[i];
        }
        // count hits WINDOW (a power of two) exactly, so the divide is a shift.
        if (++count >= WINDOW) {
            for (size_t i = 0; i < CHANNELS; i++) {
                latest[i] = static_cast<float>(sum[i] >> WINDOW_SHIFT);
            }
            sum.fill(0);
            count = 0;
        }
    }

    /// Latest completed WINDOW-sample average (per channel).
    std::array<float, CHANNELS> latestAverage() const {
        std::lock_guard<std::mutex> lock(mtx);
        return latest;
    }

private:
    mutable std::mutex mtx;
    std::array<int64_t, CHANNELS> sum{};
    std::array<float, CHANNELS> latest{};
    uint32_t count{};
};

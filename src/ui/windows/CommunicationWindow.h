#pragma once
#include "UIWindow.h"

#include <array>
#include <chrono>
#include <cstdint>

// Derives a Hz rate from a monotonically increasing counter, refreshed every ~0.5 s.
// (One per telemetry stream; lives as a window member so the rate survives across frames.)
struct RateTracker {
    uint64_t lastCount = 0;
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    double rateHz = 0.0;
    double update(uint64_t currentCount);
};

// A fixed-length "active window" for one engineering signal sampled at 100 Hz, plus a tare
// offset and a peak-hold. Replaces the unbounded plot/vector storage for the dashboard
// readouts: now() = latest - tare, avg() = mean over the last ~2 s, max() = peak-hold since
// start. The full-rate data still streams to the CSV telemetry independently of this.
struct SignalStat {
    static constexpr std::size_t WINDOW = 256;      // ring capacity (> 2 s at 100 Hz)
    static constexpr std::size_t AVG_SAMPLES = 200; // 2 s at 100 Hz
    std::array<float, WINDOW> ring{};
    std::size_t head = 0;   // index of the next write slot
    std::size_t count = 0;  // samples seen (saturates at WINDOW for ring fullness)
    float latest = 0.0f;    // most recent raw sample
    float offset = 0.0f;    // tare (subtracted from readings)
    float peak = 0.0f;      // peak-hold of (raw - offset)
    bool hasPeak = false;

    void push(float raw);   // record one raw sample; updates latest + peak-hold
    float now() const { return latest - offset; }
    float avg() const;      // mean of the last <= AVG_SAMPLES samples, tared (~2 s at 100 Hz)
    float max() const { return hasPeak ? peak : (latest - offset); }
    void tare() { offset = latest; }
    void resetPeak() { hasPeak = false; peak = 0.0f; }
};

class CommunicationWindow: public UIWindow {
public:
    const char* getName() const override;

    static const char* const name;
private:
    void renderImpl() override;

    void renderDashboardTab();
    void renderRawTab();
    void renderSetupTab();

    // --- Telemetry rate trackers (Hz derived from the GSDataCenter record counters) ---
    RateTracker ecuSsRate, ecuExtRate, fcuSsRate, fcuExtRate, gsSsRate;

    // --- Engineering-signal active windows (see kPressure/kTemp/kLoad tables in the .cpp).
    // Indexing matches those descriptor tables. ---
    std::array<SignalStat, 2> pressurePsi{};  // chamber, tank (ECU)
    std::array<SignalStat, 3> tempC{};        // Top, Throat, Tank thermistors
    std::array<SignalStat, 2> loadLb{};       // thrust (motor LC), tank load cell
    std::array<SignalStat, 2> thermocoupleC{}; // FCU MAX31856 (channels 1-2 shown)

    // --- "Time since last successful ping" (a pong received), tracked GS-side ---
    uint32_t lastPongCountSeen = 0;
    std::chrono::steady_clock::time_point lastPongTime = std::chrono::steady_clock::now();

    // --- Tank load-cell calibration masses: Empty / IPA / NOS (lb). Math TBD. ---
    float tankLcMass[3] = {0.0f, 0.0f, 0.0f};

    // --- 100 Hz sample clock for the engineering-signal windows (decoupled from frame rate) ---
    std::chrono::steady_clock::time_point lastSampleTime = std::chrono::steady_clock::now();

    // --- Setup tab (UDP) state, previously function-local statics ---
    char ipBuf[64] = {0};
    int listenerPort = 0;
    int destinationPort = 0;
    bool ipBufInitialized = false;
    bool loggingEnabled = false;
    uint64_t lostPacketCount = 0;
};

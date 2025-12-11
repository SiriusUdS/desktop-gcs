#pragma once

#include <future>
#include <mutex>

class SensorTestSequencer {
public:
    void testNOSValve();

private:
    void testNOSValveImpl();

    std::future<void> testResult;
    std::atomic_bool isTesting{};
};

#pragma once

#include "CommandTypes.h"

#include <functional>
#include <future>
#include <mutex>

class SensorTestSequencer {
public:
    bool isBusy();

    void testNOSValve();
    void testIPAValve();
    void testFillValve();
    void testDumpValve();

private:
    void tryPerformTest(std::function<void()> testFn);
    void performValveTest(CommandType valveType);

    std::future<void> testResult;
    std::atomic_bool isTesting{};
};

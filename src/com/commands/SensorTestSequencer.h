#pragma once

#include "CommandTypes.h"

#include <functional>
#include <future>
#include <mutex>

class SensorTestSequencer {
public:
    enum class TestType { NONE, NOS_VALVE, IPA_VALVE, FILL_VALVE, DUMP_VALVE };
    enum class TestAction { NONE, OPEN_VALVE, CLOSE_VALVE };

    bool isBusy();
    TestType currentTestType();
    TestAction currentTestAction();

    void testNOSValve();
    void testIPAValve();
    void testFillValve();
    void testDumpValve();

private:
    void tryPerformTest(std::function<void()> testFn);
    void performValveTest(CommandType valveType);

    std::future<void> testResult;
    std::atomic_bool isTesting{};
    std::atomic<TestType> testType{TestType::NONE};
    std::atomic<TestAction> testAction{TestAction::NONE};
};

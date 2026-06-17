#include "SensorTestSequencer.h"

#include "CommandControl.h"
#include "Logging.h"

#include <thread>

bool SensorTestSequencer::isBusy() {
    return isTesting;
}

SensorTestSequencer::TestType SensorTestSequencer::currentTestType() {
    return testType;
}

SensorTestSequencer::TestAction SensorTestSequencer::currentTestAction() {
    return testAction;
}

void SensorTestSequencer::testNOSValve() {
    tryPerformTest([this] {
        testType = TestType::NOS_VALVE;
        performValveTest(CommandType::NosValve);
    });
}

void SensorTestSequencer::testIPAValve() {
    tryPerformTest([this] {
        testType = TestType::IPA_VALVE;
        performValveTest(CommandType::IpaValve);
    });
}

void SensorTestSequencer::testFillValve() {
    tryPerformTest([this] {
        testType = TestType::FILL_VALVE;
        performValveTest(CommandType::FillValve);
    });
}

void SensorTestSequencer::testDumpValve() {
    tryPerformTest([this] {
        testType = TestType::DUMP_VALVE;
        performValveTest(CommandType::DumpValve);
    });
}

void SensorTestSequencer::tryPerformTest(std::function<void()> testFn) {
    if (isTesting.exchange(true)) {
        return;
    }

    // TODO : Make tests return actual test result and handle them
    testResult = std::async(std::launch::async, [this, testFn] {
        try {
            testFn();
        } catch (...) {
            isTesting = false;
            testType = TestType::NONE;
            testAction = TestAction::NONE;
            throw;
        }
        isTesting = false;
        testType = TestType::NONE;
        testAction = TestAction::NONE;
    });
}

void SensorTestSequencer::performValveTest(CommandType valveType) {
    std::shared_ptr<QueuedCommand> command;

    command = CommandControl::sendCommand(valveType, 100);
    testAction = TestAction::OPEN_VALVE;
    command->processed.wait(false);

    GCS_APP_LOG_DEBUG("SensorTestSequencer: Valve \"open\" command processed.", command->processed.load());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    command = CommandControl::sendCommand(valveType, 0);
    testAction = TestAction::CLOSE_VALVE;
    command->processed.wait(false);

    GCS_APP_LOG_DEBUG("SensorTestSequencer: Valve \"close\" command processed.", command->processed.load());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

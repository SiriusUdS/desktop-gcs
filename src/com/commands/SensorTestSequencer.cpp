#include "SensorTestSequencer.h"

#include "CommandControl.h"
#include "Logging.h"

#include <thread>

bool SensorTestSequencer::isBusy() {
    return isTesting;
}

void SensorTestSequencer::testNOSValve() {
    tryPerformTest(std::bind(&SensorTestSequencer::performValveTest, this, CommandType::NosValve));
}

void SensorTestSequencer::testIPAValve() {
    tryPerformTest(std::bind(&SensorTestSequencer::performValveTest, this, CommandType::IpaValve));
}

void SensorTestSequencer::testFillValve() {
    tryPerformTest(std::bind(&SensorTestSequencer::performValveTest, this, CommandType::FillValve));
}

void SensorTestSequencer::testDumpValve() {
    tryPerformTest(std::bind(&SensorTestSequencer::performValveTest, this, CommandType::DumpValve));
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
            throw;
        }
        isTesting = false;
    });
}

void SensorTestSequencer::performValveTest(CommandType valveType) {
    std::shared_ptr<QueuedCommand> command;

    command = CommandControl::sendCommand(valveType, 100);
    command->processed.wait(false);

    GCS_APP_LOG_DEBUG("SensorTestSequencer: Valve \"open\" command processed.", command->processed.load());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    command = CommandControl::sendCommand(valveType, 0);
    command->processed.wait(false);

    GCS_APP_LOG_DEBUG("SensorTestSequencer: Valve \"close\" command processed.", command->processed.load());
}

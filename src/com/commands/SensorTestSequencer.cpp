#include "SensorTestSequencer.h"

#include "CommandControl.h"
#include "CommandTypes.h"

#include <thread>

void SensorTestSequencer::testNOSValve() {
    // TODO : Make tests return actual test result
    if (!isTesting) {
        testResult = std::async(std::launch::async, [this]() { testNOSValveImpl(); });
    }
}

void SensorTestSequencer::testNOSValveImpl() {
    isTesting = true;

    std::shared_ptr<QueuedCommand> command;

    command = CommandControl::sendCommand(CommandType::NosValve, 100);
    command->processed.wait(true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    command = CommandControl::sendCommand(CommandType::NosValve, 0);
    command->processed.wait(true);

    isTesting = false;
}

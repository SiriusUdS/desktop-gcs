#include "SerialControl.h"

#include "BoardComStateMonitor.h"
#include "UdpCom.h"
#include "SerialConfig.h"
#include "SerialTask.h"
#include "Timer.h"

using namespace std::chrono;

namespace SerialControl {
constexpr size_t CONNECTION_ATTEMPT_DELAY_IN_SECS = 3;

Timer timerConnectionAttempt{seconds(CONNECTION_ATTEMPT_DELAY_IN_SECS)};
Timer timerSerialRead;
} // namespace SerialControl

void SerialControl::startComIfNeeded() {
    const bool anyBoardHasCom = SerialTask::motorBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING
                                || SerialTask::fillingStationBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING
                                || SerialTask::gsControlBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING;

    if (anyBoardHasCom) {
        return;
    }

    if (timerConnectionAttempt.hasElapsed()) {
        timerConnectionAttempt.reset();
        if (!SerialTask::com.comOpened()) {
            SerialTask::com.start();
        }
    }
}

void SerialControl::readIncomingBytesAtSetRate() {
    static constexpr size_t BYTES_TO_READ_PER_SECOND = 19'200;
    static constexpr size_t MAX_BYTES_TO_READ_PER_TASK_LOOP = 19'200 / SerialConfig::SERIAL_TASK_LOOPS_PER_SECOND;

    double elapsedSeconds = timerSerialRead.getElapsedTimeInSeconds();
    timerSerialRead.reset();

    size_t bytesToRead = std::min<size_t>(MAX_BYTES_TO_READ_PER_TASK_LOOP, (size_t) (BYTES_TO_READ_PER_SECOND * elapsedSeconds));
    while (bytesToRead--) {
        if (!SerialTask::com.read()) {
            break;
        }
    }
}

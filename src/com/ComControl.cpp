#include "ComControl.h"

#include "BoardComStateMonitor.h"
#include "ComTask.h"
#include "SerialConfig.h"
#include "Timer.h"
#include "UdpCom.h"

using namespace std::chrono;

namespace ComControl {
constexpr size_t CONNECTION_ATTEMPT_DELAY_IN_SECS = 3;

Timer timerConnectionAttempt{seconds(CONNECTION_ATTEMPT_DELAY_IN_SECS)};
Timer timerSerialRead;
} // namespace ComControl

void ComControl::startComIfNeeded() {
    const bool anyBoardHasCom = ComTask::motorBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING
                                || ComTask::fillingStationBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING
                                || ComTask::gsControlBoardComStateMonitor.getState() != BoardComStateMonitor::State::NOT_WORKING;

    if (anyBoardHasCom) {
        return;
    }

    if (timerConnectionAttempt.hasElapsed()) {
        timerConnectionAttempt.reset();
        if (!ComTask::com->comOpened()) {
            ComTask::com->start();
        }
    }
}

void ComControl::readIncomingBytesAtSetRate() {
    ComTask::com->read();
}

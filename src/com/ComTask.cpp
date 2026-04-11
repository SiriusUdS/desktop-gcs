#include "ComTask.h"

#include "BoardComStateMonitor.h"
#include "CommandControl.h"
#include "IntervalTimer.h"
#include "PacketProcessing.h"
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"
#include "UdpCom.h"
#include "SerialConfig.h"
#include "ComControl.h"

namespace ComTask {
PacketRateMonitor packetRateMonitor;
PacketRateMonitor engineTelemetryPacketRateMonitor;
PacketRateMonitor fillingStationTelemetryPacketRateMonitor;
PacketRateMonitor gsControlPacketRateMonitor;
PacketRateMonitor engineStatusPacketRateMonitor;
PacketRateMonitor fillingStationStatusPacketRateMonitor;
PacketReceiver packetReceiver;
BoardComStateMonitor motorBoardComStateMonitor;
BoardComStateMonitor fillingStationBoardComStateMonitor;
BoardComStateMonitor gsControlBoardComStateMonitor;
std::unique_ptr<ICom> com = std::make_unique<UdpCom>(); //Switch to SerialCom here

IntervalTimer intervalTimer(std::chrono::milliseconds(1000 / SerialConfig::SERIAL_TASK_LOOPS_PER_SECOND));
std::thread thread;
std::chrono::steady_clock::time_point timeLastUpdate = std::chrono::steady_clock::now();
std::atomic<bool> running = false;
std::atomic<bool> shouldStop = false;
} // namespace ComTask

void ComTask::start() {
    if (running) {
        return;
    }

    shouldStop = false;
    thread = std::thread(&ComTask::execute);
    running = true;
}

void ComTask::execute() {
    while (!shouldStop) {
        intervalTimer.waitUntilNextInterval();
        ComControl::startComIfNeeded();
        ComControl::readIncomingBytesAtSetRate();
        PacketProcessing::processIncomingPackets();
        CommandControl::processCommands();
    }
}

void ComTask::restart() {
    stop();
    start();
}

void ComTask::stop() {
    if (!running) {
        return;
    }

    shouldStop = true;
    if (thread.joinable()) {
        thread.join();
    }
    running = false;
}

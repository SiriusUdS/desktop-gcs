#include "SerialTask.h"

#include "BoardComStateMonitor.h"
#include "ComPortSelector.h"
#include "CommandControl.h"
#include "IntervalTimer.h"
#include "PacketProcessing.h"
#include "PacketRateMonitor.h"
#include "PacketReceiver.h"
#include "UdpCom.h"
#include "SerialConfig.h"
#include "SerialControl.h"
#include "WindowsComPortDiscovery.h"

namespace SerialTask {
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
WindowsComPortDiscovery comPortDiscovery;
ComPortSelector comPortSelector(comPortDiscovery);
std::unique_ptr<ICom> com = std::make_unique<UdpCom>();

IntervalTimer intervalTimer(std::chrono::milliseconds(1000 / SerialConfig::SERIAL_TASK_LOOPS_PER_SECOND));
std::thread thread;
std::chrono::steady_clock::time_point timeLastUpdate = std::chrono::steady_clock::now();
std::atomic<bool> running = false;
std::atomic<bool> shouldStop = false;
} // namespace SerialTask

void SerialTask::start() {
    if (running) {
        return;
    }

    shouldStop = false;
    thread = std::thread(&SerialTask::execute);
    running = true;
}

void SerialTask::execute() {
    while (!shouldStop) {
        intervalTimer.waitUntilNextInterval();
        SerialControl::startComIfNeeded();
        SerialControl::readIncomingBytesAtSetRate();
        PacketProcessing::processIncomingPackets();
        CommandControl::processCommands();
    }
}

void SerialTask::restart() {
    stop();
    start();
}

void SerialTask::stop() {
    if (!running) {
        return;
    }

    shouldStop = true;
    if (thread.joinable()) {
        thread.join();
    }
    running = false;
}

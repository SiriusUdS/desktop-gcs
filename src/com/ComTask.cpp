#include "ComTask.h"

#include "BoardComStateMonitor.h"
#include "ComControl.h"
#include "CommandControl.h"
#include "DeviceTracker.h"
#include "IntervalTimer.h"
#include "PacketProcessing.h"
#include "PacketRateMonitor.h"
#include "SerialConfig.h"
#include "UdpCom.h"
#include "UdpPacketReceiver.h"

namespace ComTask {
PacketRateMonitor packetRateMonitor;
PacketRateMonitor engineTelemetryPacketRateMonitor;
PacketRateMonitor fillingStationTelemetryPacketRateMonitor;
PacketRateMonitor gsControlPacketRateMonitor;
PacketRateMonitor engineStatusPacketRateMonitor;
PacketRateMonitor fillingStationStatusPacketRateMonitor;
UdpPacketReceiver udpPacketReceiver;
BoardComStateMonitor motorBoardComStateMonitor;
BoardComStateMonitor fillingStationBoardComStateMonitor;
BoardComStateMonitor gsControlBoardComStateMonitor;
std::unique_ptr<ICom> com;
std::unique_ptr<DeviceTracker> deviceTracker;

IntervalTimer intervalTimer(std::chrono::milliseconds(1000 / SerialConfig::SERIAL_TASK_LOOPS_PER_SECOND));
std::thread thread;
std::chrono::steady_clock::time_point timeLastUpdate = std::chrono::steady_clock::now();
std::atomic<bool> running = false;
std::atomic<bool> shouldStop = false;
} // namespace ComTask

void ComTask::start(std::unique_ptr<ICom> comInterface) {
    com = std::move(comInterface);
    if (running) {
        return;
    }

    if (com.get()->getComType() == ComType::UDP) {
        deviceTracker = std::make_unique<DeviceTracker>();
    }
    
    shouldStop = false;
    thread = std::thread(&ComTask::execute);
    running = true;
}

void ComTask::execute() {
    while (!shouldStop) {
        intervalTimer.waitUntilNextInterval();
        ComControl::startComIfNeeded();
        com->read();
        PacketProcessing::processIncomingPackets();
        CommandControl::processCommands();
    }
}

void ComTask::restart() {
    stop();
    start(std::move(com));
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

bool ComTask::updateConnection(std::string ipAddress, uint16_t sendPort, uint16_t receivePort) {
    if (com.get()->getComType() != ComType::UDP) {
        return false;
    }
    
    return dynamic_cast<UdpCom*>(com.get())->openConnectionTo(ipAddress, sendPort, receivePort);
}

uint64_t ComTask::getLostPacketCount() {
    return udpPacketReceiver.getAmountOfLostPackets();
}

uint64_t ComTask::getTotalReceivedPackets() {
    return udpPacketReceiver.getAmountOfReceivedPackets();
}

void ComTask::updateUDPDevice(DeviceInformation deviceInformation) {
        if (deviceTracker) {
                deviceTracker->updateDevice(deviceInformation.deviceID, deviceInformation);
        }
}

std::vector<DeviceInformation> ComTask::getAllDeviceInformation() {
    if (deviceTracker) {
        return deviceTracker->getAllDeviceInformation();
    }
    
    return {};
}

#include "ComTask.h"

#include "BoardComStateMonitor.h"
#include "ComControl.h"
#include "CommandControl.h"
#include "CRC.h"
#include "DeviceTracker.h"
#include "GSDataCenter.h"
#include "IntervalTimer.h"
#include "PacketProcessing.h"
#include "PacketRateMonitor.h"
#include "SerialConfig.h"
#include "Timer.h"
#include "UdpCom.h"
#include "UdpPacketReceiver.h"

#include "framing/ethernet_header.hpp"
#include "framing/payload_type.hpp"
#include "system/board_id.hpp"
#include "telemetry/gs_system_state.hpp"
#include "telemetry/telemetry_type.hpp"

#include <cstring>
#include <span>

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
std::atomic<bool> streamGsSystemState = true; // stream GsSystemState to the board by default
std::atomic<bool> autoPing = false;           // off by default; opt-in 1 Hz Ping heartbeat
} // namespace ComTask

namespace {
// Cadence for the outbound GsSystemState stream (10 Hz).
Timer gsSystemStateSendTimer(std::chrono::milliseconds(100));

// Cadence for the auto-ping heartbeat (1 Hz).
Timer pingSendTimer(std::chrono::milliseconds(1000));

// Build one GSSystemState telemetry frame (addressed to board id 3 / GsControl) and send it to
// the configured destination. The last 2 bytes carry the current FCU/ECU board states (the most
// recent ones decoded from their SystemState telemetry); the switch/button bitfields stay clear.
void sendGsSystemStateFrame() {
    GSSystemState gs{};   // zero-init leaves the switch/button bitfields (first 2 bytes) clear
    gs.fcuState = GSDataCenter::fillingStationBoardState.load(); // FCU = filling-station board
    gs.ecuState = GSDataCenter::motorBoardState.load();          // ECU = motor/engine board

    uint8_t frame[sizeof(EthernetHeader) + sizeof(GSSystemState) + sizeof(uint32_t)] = {0};
    EthernetHeader header{};
    header.sender_id = static_cast<uint32_t>(BoardId::GsControl);
    header.target_id = static_cast<uint32_t>(BoardId::GsControl); // board id 3
    header.payload_type = static_cast<uint32_t>(PayloadType::Telemetry);
    header.payload_id = static_cast<uint32_t>(TelemetryType::SystemState);
    header.payload_size_bytes = sizeof(GSSystemState);

    std::memcpy(frame, &header, sizeof(header));
    std::memcpy(frame + sizeof(header), &gs, sizeof(gs));
    const size_t crcRange = sizeof(header) + sizeof(gs);
    const uint32_t crc = CRC::computeCrc32(frame, crcRange);
    std::memcpy(frame + crcRange, &crc, sizeof(crc));

    ComTask::com->write(std::span<const uint8_t>(frame, sizeof(frame)));
}
} // namespace

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
        // Queue one Ping per second when the heartbeat is on; the processCommands() call just
        // below flushes it this same iteration, like any other command.
        if (autoPing && com && com->comOpened() && pingSendTimer.hasElapsed()) {
            pingSendTimer.reset();
            CommandControl::sendCommand(CommandType::Ping, 0);
        }
        // Commands take priority: flush any pending command before spending the loop on telemetry.
        CommandControl::processCommands();
        if (streamGsSystemState && com && com->comOpened() && gsSystemStateSendTimer.hasElapsed()) {
            gsSystemStateSendTimer.reset();
            sendGsSystemStateFrame();
        }
        com->read();
        PacketProcessing::processIncomingPackets();
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

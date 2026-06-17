#pragma once

#include "DeviceInformation.h"
#include "DeviceTracker.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <thread>


class BoardComStateMonitor;

class IntervalTimer;
class PacketRateMonitor;
class ICom;
class UdpPacketReceiver;

namespace ComTask {
void start(std::unique_ptr<ICom> comInterface);
void execute();
void restart();
void stop();
bool updateConnection(std::string ipAddress, uint16_t sendPort, uint16_t receivePort);
uint64_t getLostPacketCount();
uint64_t getTotalReceivedPackets();
void updateUDPDevice(DeviceInformation deviceInformation);
std::vector<DeviceInformation> getAllDeviceInformation();

extern PacketRateMonitor packetRateMonitor;
extern PacketRateMonitor engineTelemetryPacketRateMonitor;
extern PacketRateMonitor fillingStationTelemetryPacketRateMonitor;
extern PacketRateMonitor gsControlPacketRateMonitor;
extern PacketRateMonitor engineStatusPacketRateMonitor;
extern PacketRateMonitor fillingStationStatusPacketRateMonitor;
extern UdpPacketReceiver udpPacketReceiver;
extern BoardComStateMonitor motorBoardComStateMonitor;
extern BoardComStateMonitor fillingStationBoardComStateMonitor;
extern BoardComStateMonitor gsControlBoardComStateMonitor;
extern std::unique_ptr<ICom> com;
extern std::unique_ptr<DeviceTracker> deviceTracker;

// When true, the com loop streams a GSSystemState frame to the board on a fixed cadence
// (addressed to board id 3 / GsControl), sent to the configured destination.
extern std::atomic<bool> streamGsSystemState;

// When true, the com loop queues one Ping command per second (a manual heartbeat the
// boards report back via ExtendedSystemState::seconds_since_last_ping).
extern std::atomic<bool> autoPing;
} // namespace ComTask

#pragma once

#include <atomic>
#include <string>
#include <thread>


class BoardComStateMonitor;

class IntervalTimer;
class PacketRateMonitor;
class PacketReceiver;
class ICom;
class UdpPacketReceiver;

namespace ComTask {
void start(std::unique_ptr<ICom> comInterface);
void execute();
void restart();
void stop();
bool updateConnection(std::string ipAddress, int port);
int getLostPacketCount();

extern PacketRateMonitor packetRateMonitor;
extern PacketRateMonitor engineTelemetryPacketRateMonitor;
extern PacketRateMonitor fillingStationTelemetryPacketRateMonitor;
extern PacketRateMonitor gsControlPacketRateMonitor;
extern PacketRateMonitor engineStatusPacketRateMonitor;
extern PacketRateMonitor fillingStationStatusPacketRateMonitor;
extern PacketReceiver packetReceiver;
extern UdpPacketReceiver udpPacketReceiver;
extern BoardComStateMonitor motorBoardComStateMonitor;
extern BoardComStateMonitor fillingStationBoardComStateMonitor;
extern BoardComStateMonitor gsControlBoardComStateMonitor;
extern std::unique_ptr<ICom> com;
} // namespace ComTask

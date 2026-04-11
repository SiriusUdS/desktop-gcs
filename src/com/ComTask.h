#pragma once

#include <atomic>
#include <thread>


class BoardComStateMonitor;

class IntervalTimer;
class PacketRateMonitor;
class PacketReceiver;
class ICom;

namespace ComTask {
void start();
void execute();
void restart();
void stop();

extern PacketRateMonitor packetRateMonitor;
extern PacketRateMonitor engineTelemetryPacketRateMonitor;
extern PacketRateMonitor fillingStationTelemetryPacketRateMonitor;
extern PacketRateMonitor gsControlPacketRateMonitor;
extern PacketRateMonitor engineStatusPacketRateMonitor;
extern PacketRateMonitor fillingStationStatusPacketRateMonitor;
extern PacketReceiver packetReceiver;
extern BoardComStateMonitor motorBoardComStateMonitor;
extern BoardComStateMonitor fillingStationBoardComStateMonitor;
extern BoardComStateMonitor gsControlBoardComStateMonitor;
extern std::unique_ptr<ICom> com;
} // namespace SerialTask

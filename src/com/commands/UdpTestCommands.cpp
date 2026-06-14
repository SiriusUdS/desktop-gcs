#include "UdpTestCommands.h"

#include "Logging.h"

void UdpTestCommands::testValve(uint16_t valueToSend) {
    // TODO: rebuild on the common-protocol command SSOT — a PayloadType::Command
    // frame carrying CommandType::SetValvePosition with a SetValvePositionFrame
    // payload (valve selector + ValveCommand::SetOpenedPct + value). The old
    // ServoPacket / SET_SERVO / SERVO_MANUAL constructs are GCS-local, not part of
    // the protocol submodule (the SSOT), so they were dropped here. This needs a
    // valve selector — the Test-Over-UDP window currently sends only a value.
    (void)valueToSend;
    GCS_APP_LOG_WARN("UdpTestCommands: testValve is disabled pending migration to the common-protocol SetValvePosition command.");
}

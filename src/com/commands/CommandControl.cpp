#include "CommandControl.h"

#include "ComTask.h"
#include "CommandQueue.h"
#include "CRC.h"
#include "Logging.h"
#include "UdpCom.h"
#include "Timer.h"

#include <cstring>
#include <span>

namespace CommandControl {
/**
 * @enum State
 * @brief Represents the state of the command control.
 */
enum class State {
    IDLE,    ///< No command is being sent
    SENDING, ///< A command is being sent
};

constexpr size_t MAX_DATA_SIZE = 256;                       ///< Maximum size of the command's data
constexpr size_t NUMBER_OF_TIMES_TO_SEND_SAME_COMMAND = 1; ///< Each command is sent this many times to improve communication with the boards
constexpr double TIME_BETWEEN_COMMAND_SENDS_SEC = 0.010;     ///< Wait this much between each command send (UDP: tight 10 ms cadence)

using SsCommandType = logic::communication::command::CommandType; ///< On-wire (SSOT) command id

State state = State::IDLE;                                    ///< Current state of the command
uint8_t data[MAX_DATA_SIZE] = {0};                           ///< Current command frame bytes
size_t dataSize;                                             ///< Size of the current command frame
Timer lastTimeSentTimer;                                     ///< Timer for the last time the command was sent
size_t timesSent{};                                          ///< Number of times the same command has been sent
CommandQueue commandQueue;                                   ///< Queue containing all future commands to be sent
std::optional<std::shared_ptr<QueuedCommand>> currentCommand; ///< Current command being sent

void getNextCommand();
void completeCurrentCommand();
void setupValveCommand(BoardId target, uint8_t valveIndex);
void setupSolenoidValveCommand(uint8_t on);
void setupHeaterCommand(uint8_t on);
void setupSetControlFlagCommand(uint16_t flagId, uint8_t on);
void setupSetStateCommand(uint8_t requestedStateId);
void setupPing();
void stubUnimplementedCommand(const char* what);
} // namespace CommandControl

const std::shared_ptr<QueuedCommand> CommandControl::sendCommand(CommandType type, uint32_t value) {
    return commandQueue.enqueue(type, value);
}

namespace {
// Build an outbound command frame into `out`: EthernetHeader (12 B) + payload
// (zero-padded up to a 4-byte multiple) + standard CRC-32 (4 B, little-endian).
// The CRC covers the EthernetHeader and the padded payload; the UDP/IP transport
// header is excluded (it carries its own checksum).

// Per-command sequence number. The seq tags a logical command so its reply can be
// matched and the command retried; every resend of the same command shares one seq,
// so it advances once per built frame (not per transmit). The boards treat seq as a
// 4-bit field, so it stays in 1..15 (0 is reserved for telemetry / unset).
uint8_t nextCommandSeq() {
    static uint8_t seq = 0;
    seq = (seq + 1) & 0x0F; // 4-bit field on the boards: wrap within 0..15
    if (seq == 0) {         // skip 0 — it denotes telemetry / unset
        seq = 1;
    }
    return seq;
}

size_t buildCommandFrame(uint8_t* out, BoardId target, uint8_t payloadId, const uint8_t* payload, size_t payloadLen) {
    const size_t paddedLen = (payloadLen + 3u) & ~size_t(3u);

    EthernetHeader header{};
    header.sender_id = static_cast<uint32_t>(BoardId::GsControl);
    header.target_id = static_cast<uint32_t>(target);
    header.payload_type = static_cast<uint32_t>(PayloadType::Command);
    header.payload_id = payloadId;
    header.payload_size_bytes = static_cast<uint32_t>(paddedLen);
    header.sender_state = 0;
    header.seq = nextCommandSeq();  // tags this command for reply-matching / retry (shared across its resends)
    header.sender_timestamp_ms = 0; // empty for GS commands per the EthernetHeader spec

    std::memcpy(out, &header, sizeof(header));
    std::memset(out + sizeof(header), 0, paddedLen);
    if (payloadLen > 0) {
        std::memcpy(out + sizeof(header), payload, payloadLen);
    }

    const size_t crcRange = sizeof(header) + paddedLen;
    const uint32_t crc = CRC::computeCrc32(out, crcRange);
    std::memcpy(out + crcRange, &crc, sizeof(crc));

    return crcRange + sizeof(crc);
}
} // namespace

void CommandControl::processCommands() {
    // Preempt: a freshly queued command supersedes the in-flight resend burst so the
    // latest operator intent goes out on the next loop, instead of waiting up to ~1 s
    // for the current command's 100 resends to drain. The abandoned command is still
    // completed (below) so any waiter is released.
    if (state == State::SENDING && !commandQueue.empty()) {
        completeCurrentCommand();
        state = State::IDLE;
    }

    if (state == State::IDLE) {
        getNextCommand(); // dequeue + set up; transitions to SENDING for a real command (stubs stay IDLE)
        if (state != State::SENDING) {
            return;
        }
        timesSent = 0;    // new command: transmit its first copy immediately (no extra loop of latency)
    } else if (lastTimeSentTimer.getElapsedTimeInSeconds() < TIME_BETWEEN_COMMAND_SENDS_SEC) {
        return;           // mid-resend: wait for the inter-send interval
    }

    lastTimeSentTimer.reset();
    if (!ComTask::com->write(std::span<const uint8_t>(data, dataSize))) {
        std::string protocolName = ComTask::com->getProtocolName();
        GCS_APP_LOG_ERROR("CommandControl: Couldn't send command over {} communication.", protocolName);
    }
    timesSent++;

    if (NUMBER_OF_TIMES_TO_SEND_SAME_COMMAND <= timesSent) {
        completeCurrentCommand();
        state = State::IDLE;
        timesSent = 0;
    }
}

// Mark the in-flight command processed and release any waiter (e.g. SensorTestSequencer),
// then clear it. Safe to call with no command in flight.
void CommandControl::completeCurrentCommand() {
    if (currentCommand.has_value()) {
        currentCommand.value()->processed = true;
        currentCommand.value()->processed.notify_one();
        currentCommand = std::nullopt;
    }
}

void CommandControl::getNextCommand() {
    currentCommand = commandQueue.dequeue();

    if (!currentCommand.has_value()) {
        return;
    }

    switch (currentCommand.value()->type) {
    case CommandType::NosValve:
        setupValveCommand(BoardId::Engine, static_cast<uint8_t>(EcuValves::NOS));
        break;
    case CommandType::IpaValve:
        setupValveCommand(BoardId::Engine, static_cast<uint8_t>(EcuValves::IPA));
        break;
    case CommandType::FillValve:
        setupValveCommand(BoardId::FillingStation, static_cast<uint8_t>(FcuValves::Fill));
        break;
    case CommandType::DumpValve:
        setupValveCommand(BoardId::FillingStation, static_cast<uint8_t>(FcuValves::Dump));
        break;
    case CommandType::NosHeatPad:
    case CommandType::IpaHeatPad:
    case CommandType::FillHeatPad:
    case CommandType::DumpHeatPad:
        // TODO: heat-pad / solenoid commands are not in the common-protocol yet.
        // The FCU will eventually expose 1 heater + 1 solenoid valve; re-add then.
        stubUnimplementedCommand("Heat-pad / solenoid");
        break;
    case CommandType::Abort:
    case CommandType::Reset:
        // TODO: map to SetState (Abort state / SET_STATE_FLAG_RESET) once the
        // state-command semantics and target are finalized.
        stubUnimplementedCommand("Abort / Reset");
        break;
    case CommandType::SetState:
        setupSetStateCommand(static_cast<uint8_t>(currentCommand.value()->value));
        break;
    case CommandType::SolenoidValve:
        setupSolenoidValveCommand(static_cast<uint8_t>(currentCommand.value()->value));
        break;
    case CommandType::Heater:
        setupHeaterCommand(static_cast<uint8_t>(currentCommand.value()->value));
        break;
    case CommandType::SetControlFlag: {
        // value packs the 16-bit global flag id and the on/off bit: (flagId << 1) | (on & 1).
        const uint32_t packed = currentCommand.value()->value;
        setupSetControlFlagCommand(static_cast<uint16_t>(packed >> 1), static_cast<uint8_t>(packed & 1u));
        break;
    }
    case CommandType::Ping:
        setupPing();
        break;
    default:
        GCS_APP_LOG_ERROR("CommandControl: Invalid command type dequeued from command queue. Ignoring command.");
        stubUnimplementedCommand("Unknown");
    }
}

void CommandControl::setupValveCommand(BoardId target, uint8_t valveIndex) {
    if (!currentCommand.has_value()) {
        GCS_APP_LOG_ERROR("CommandControl: Couldn't setup valve command, no command available.");
        return;
    }

    const uint32_t value = currentCommand.value()->value;
    const bool forced = (value & VALVE_FORCE_FLAG) != 0;
    uint32_t percentageOpen = value & ~VALVE_FORCE_FLAG;

    if (percentageOpen > 100) {
        GCS_APP_LOG_WARN("CommandControl: Invalid valve percentage: {}. Must be between 0 and 100.", percentageOpen);
        stubUnimplementedCommand("Out-of-range valve");
        return;
    }

    SetValvePositionFrame frame{};
    frame.valve = static_cast<FcuValves>(valveIndex); // on-wire value is the per-board valve index (EcuValves / FcuValves)
    frame.force = forced ? 1 : 0; // bypass the limit switches for this actuation (Open/Close only; ignored for SetOpenedPct)
    // The ECU treats valves as binary (opened/closed) and rejects SetOpenedPct, so map
    // the endpoints to the discrete Open/Close actions; only intermediate positions use the percentage.
    if (percentageOpen >= 100) {
        frame.action = ValveCommand::Open;
        frame.value = 100;
    } else if (percentageOpen == 0) {
        frame.action = ValveCommand::Close;
        frame.value = 0;
    } else {
        frame.action = ValveCommand::SetOpenedPct;
        frame.value = static_cast<uint8_t>(percentageOpen);
    }

    dataSize = buildCommandFrame(data,
                                 target,
                                 static_cast<uint8_t>(SsCommandType::SetValvePosition),
                                 reinterpret_cast<const uint8_t*>(&frame),
                                 sizeof(frame));
    state = State::SENDING;
}

// Open (set) or close (clear) the FCU solenoid valve via a SetControlFlag command.
// The solenoid is a per-board flag, so its global on-wire id is the board offset plus
// the FcuControlFlag bit. The board gates actuation to Unsafe and auto-clears the flag
// on leaving it; the GS just toggles the flag.
void CommandControl::setupSolenoidValveCommand(uint8_t on) {
    SetControlFlagFrame frame{};
    frame.flag = CONTROL_FLAG_BOARD_OFFSET + static_cast<uint16_t>(FcuControlFlag::SolenoidValve);
    frame.value = on ? 1 : 0;

    dataSize = buildCommandFrame(data,
                                 BoardId::FillingStation,
                                 static_cast<uint8_t>(SsCommandType::SetControlFlag),
                                 reinterpret_cast<const uint8_t*>(&frame),
                                 sizeof(frame));
    state = State::SENDING;
}

// Turn the FCU heater on (set) or off (clear) via a SetControlFlag command. Like the
// solenoid it is a per-board flag, so its global on-wire id is the board offset plus the
// FcuControlFlag bit. Unlike the solenoid it is not state-gated: the FCU follows the flag
// in any state, so the GS just toggles it.
void CommandControl::setupHeaterCommand(uint8_t on) {
    SetControlFlagFrame frame{};
    frame.flag = CONTROL_FLAG_BOARD_OFFSET + static_cast<uint16_t>(FcuControlFlag::HeaterTank);
    frame.value = on ? 1 : 0;

    dataSize = buildCommandFrame(data,
                                 BoardId::FillingStation,
                                 static_cast<uint8_t>(SsCommandType::SetControlFlag),
                                 reinterpret_cast<const uint8_t*>(&frame),
                                 sizeof(frame));
    state = State::SENDING;
}

// Set (or clear) a control flag by its 16-bit global id. Broadcast: the persistence
// flags are BASE flags common to every board (ids 0..7), so every board toggles together.
// Per-board flags (ids >= 8) would still be accepted by their owning board on a broadcast.
void CommandControl::setupSetControlFlagCommand(uint16_t flagId, uint8_t on) {
    SetControlFlagFrame frame{};
    frame.flag = flagId;
    frame.value = on ? 1 : 0;

    dataSize = buildCommandFrame(data,
                                 BoardId::Broadcast,
                                 static_cast<uint8_t>(SsCommandType::SetControlFlag),
                                 reinterpret_cast<const uint8_t*>(&frame),
                                 sizeof(frame));
    state = State::SENDING;
}

void CommandControl::setupSetStateCommand(uint8_t requestedStateId) {
    SetStateFrame frame{};
    frame.flags = 0; // no reply flags: the command is ACKed, and the resulting state arrives with telemetry
    frame.requestedID = requestedStateId;

    // Broadcast: the state machine is network-wide, so every board transitions together.
    dataSize = buildCommandFrame(data,
                                 BoardId::Broadcast,
                                 static_cast<uint8_t>(SsCommandType::SetState),
                                 reinterpret_cast<const uint8_t*>(&frame),
                                 sizeof(frame));
    state = State::SENDING;
}

void CommandControl::setupPing() {
    // Ping carries no payload; the addressed board(s) reply with a Pong (Response).
    dataSize = buildCommandFrame(data,
                                 BoardId::Broadcast,
                                 static_cast<uint8_t>(SsCommandType::Ping),
                                 nullptr,
                                 0);
    state = State::SENDING;
}

// Drop a command we can't build on the common-protocol yet, completing it so the
// queue keeps moving (the controls stay; their effect is a no-op for now).
void CommandControl::stubUnimplementedCommand(const char* what) {
    GCS_APP_LOG_WARN("CommandControl: {} command is not implemented for the common-protocol yet; dropping.", what);
    completeCurrentCommand();
    state = State::IDLE;
}

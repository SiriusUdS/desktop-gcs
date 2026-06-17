#pragma once

#include "command/command_type.hpp"
#include "framing/ethernet_header.hpp"
#include "framing/payload_type.hpp"
#include "system/board_id.hpp"
#include "system/valves/ecu.hpp"
#include "system/valves/fcu.hpp"

#include <atomic>
#include <cstdint>

/**
 * @enum CommandType
 * @brief GCS-side UI command selector — identifies which control the operator
 *        actuated. Distinct from the on-wire SSOT command id
 *        (logic::communication::command::CommandType): CommandControl maps these
 *        to the protocol commands. Heat-pad / abort / reset entries are kept so
 *        their controls still compile, but are stubbed in CommandControl until
 *        the protocol provides them.
 */
enum class CommandType : size_t {
    NosValve,
    IpaValve,
    FillValve,
    DumpValve,
    NosHeatPad,
    IpaHeatPad,
    FillHeatPad,
    DumpHeatPad,
    Abort,
    Reset,
    Ping,
    SetState,      // value carries the requested logic::control::State id
    SolenoidValve, // FCU solenoid via SetControlFlag; value carries 0 = close (clear flag), 1 = open (set flag)
    Heater,        // FCU heater via SetControlFlag; value carries 0 = off (clear flag), 1 = on (set flag)
    SetControlFlag, // Generic SetControlFlag (broadcast); value packs the 16-bit global flag id and on/off
                    // as (flagId << 1) | (on & 1) -- see CommandControl::setupSetControlFlagCommand.

    Count // Always keep this as the last element
};

// Bit OR'd into a valve command's value to request a FORCED actuation (the board bypasses the
// limit switches for FORCED_VALVE_ACTUATION_MS, then auto-reverts). The low byte carries the
// 0..100 opened-percentage; this bit rides above it. Applies to Open/Close; the board ignores
// it for intermediate (SetOpenedPct) positions.
inline constexpr uint32_t VALVE_FORCE_FLAG = 0x100u;

/**
 * @struct QueuedCommand
 * @brief Represents a command with its type and value.
 */
struct QueuedCommand {
    CommandType type;             ///< Type of the command.
    uint32_t value;               ///< Value of the command.
    std::atomic_bool processed{}; ///< Whether the command was processed or not.
};

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
    SetState, // value carries the requested logic::control::State id

    Count // Always keep this as the last element
};

/**
 * @struct QueuedCommand
 * @brief Represents a command with its type and value.
 */
struct QueuedCommand {
    CommandType type;             ///< Type of the command.
    uint32_t value;               ///< Value of the command.
    std::atomic_bool processed{}; ///< Whether the command was processed or not.
};

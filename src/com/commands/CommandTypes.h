#pragma once

#include "Telecommunication/BoardCommand.h"
#include "Telecommunication/PacketHeaderVariable.h"

#include <atomic>

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

enum class ValveCommandType {
    Nos = ENGINE_COMMAND_CODE_OPEN_NOS_VALVE_PCT,
    Ipa = ENGINE_COMMAND_CODE_OPEN_IPA_VALVE_PCT,
    Fill = FILLING_STATION_COMMAND_CODE_OPEN_FILL_VALVE_PCT,
    Dump = FILLING_STATION_COMMAND_CODE_OPEN_DUMP_VALVE_PCT
};

enum class HeatPadCommandType {
    Nos = ENGINE_COMMAND_CODE_SET_NOS_VALVE_HEATER_POWER_PCT,
    Ipa = ENGINE_COMMAND_CODE_SET_IPA_VALVE_HEATER_POWER_PCT,
    Fill = FILLING_STATION_COMMAND_CODE_SET_FILL_VALVE_HEATER_POWER_PCT,
    Dump = FILLING_STATION_COMMAND_CODE_SET_DUMP_VALVE_HEATER_POWER_PCT
};

enum class BoardType { Engine = ENGINE_BOARD_ID, FillingStation = FILLING_STATION_BOARD_ID };

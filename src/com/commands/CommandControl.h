#pragma once

#include "CommandTypes.h"

namespace CommandControl {
void sendCommand(CommandType type, uint32_t value);
void processCommands();
} // namespace CommandControl

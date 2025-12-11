#pragma once

#include "CommandTypes.h"

#include <memory>

namespace CommandControl {
const std::shared_ptr<QueuedCommand> sendCommand(CommandType type, uint32_t value);
void processCommands();
} // namespace CommandControl

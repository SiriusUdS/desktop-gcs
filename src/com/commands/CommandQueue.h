#pragma once

#include "CommandTypes.h"

#include <array>
#include <optional>
#include <queue>

/**
 * @class CommandQueue
 * @brief A queue for commands to be processed later.
 * Queueing is implemented as a last-writer-wins strategy, meaning that if the same command type is enqueued multiple times before
 * its processed, the last value written will be the one processed when dequeuing.
 */
class CommandQueue {
public:
    void enqueue(CommandType type, uint32_t value);
    std::optional<Command> dequeue();
    bool empty() const;

private:
    std::queue<CommandType> pendingTypes;
    std::array<std::optional<uint32_t>, static_cast<std::size_t>(CommandType::Count)> values{};
};

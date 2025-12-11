#pragma once

#include "CommandTypes.h"

#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>

/**
 * @class CommandQueue
 * @brief A queue for commands to be processed later.
 * Queueing is implemented as a last-writer-wins strategy, meaning that if the same command type is enqueued multiple times before
 * its processed, the last value written will be the one processed when dequeuing.
 */
class CommandQueue {
public:
    const std::shared_ptr<QueuedCommand> enqueue(CommandType type, uint32_t value);
    std::optional<std::shared_ptr<QueuedCommand>> dequeue();
    bool empty() const;

private:
    std::queue<CommandType> queue;
    std::unordered_map<std::underlying_type_t<CommandType>, std::shared_ptr<QueuedCommand>> active;
};

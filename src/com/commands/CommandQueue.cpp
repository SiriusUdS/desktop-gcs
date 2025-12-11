#include "CommandQueue.h"

#include "Logging.h"

/**
 * @brief Queues commands for later processing.
 * @param type The type of command to enqueue.
 * @param value The value associated with the command.
 */
const std::shared_ptr<QueuedCommand> CommandQueue::enqueue(CommandType type, uint32_t value) {
    auto typeIdx = static_cast<std::underlying_type_t<CommandType>>(type);

    if (active.find(typeIdx) == active.end()) {
        queue.push(type);
    }

    auto command = std::make_shared<QueuedCommand>();
    command->type = type;
    command->value = value;
    active[typeIdx] = command;

    return command;
}

/**
 * @brief Dequeues the next command from the queue.
 * @returns The next command if one is available, else returns std::nullopt.
 */
std::optional<std::shared_ptr<QueuedCommand>> CommandQueue::dequeue() {
    if (queue.empty()) {
        return std::nullopt;
    }

    CommandType type = queue.front();
    queue.pop();

    auto idx = static_cast<std::underlying_type_t<CommandType>>(type);

    auto it = active.find(idx);
    if (it == active.end()) {
        GCS_APP_LOG_WARN("CommandQueue: Desynchronization between command queue and active commands map. This should never happend.");
        return std::nullopt;
    }

    auto cmd = it->second;
    active.erase(it);

    return cmd;
}

/**
 * @brief Checks if the command queue is empty.
 * @returns True if the queue is empty, false otherwise.
 */
bool CommandQueue::empty() const {
    return active.empty();
}

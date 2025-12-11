#include "CommandQueue.h"

#include <doctest.h>

void checkNoCommand(std::optional<std::shared_ptr<QueuedCommand>> optionalCmd) {
    CHECK(!optionalCmd.has_value());
}

void checkCommand(std::optional<std::shared_ptr<QueuedCommand>> optionalCmd, CommandType type, uint32_t value) {
    if (!optionalCmd.has_value()) {
        CHECK(false);
        return;
    }
    std::shared_ptr<QueuedCommand> cmd = optionalCmd.value();
    CHECK(type == cmd->type);
    CHECK(value == cmd->value);
}

TEST_CASE("CommandQueue should not return command if dequeuing and no command was queued") {
    CommandQueue queue;
    checkNoCommand(queue.dequeue());
    checkNoCommand(queue.dequeue());
}

TEST_CASE("CommandQueue should dequeue commands in the order they were queued") {
    CommandQueue queue;
    queue.enqueue(CommandType::FillValve, 1);
    queue.enqueue(CommandType::DumpHeatPad, 2);
    queue.enqueue(CommandType::FillHeatPad, 5);
    queue.enqueue(CommandType::DumpValve, 10);
    checkCommand(queue.dequeue(), CommandType::FillValve, 1);
    checkCommand(queue.dequeue(), CommandType::DumpHeatPad, 2);
    checkCommand(queue.dequeue(), CommandType::FillHeatPad, 5);
    checkCommand(queue.dequeue(), CommandType::DumpValve, 10);
    checkNoCommand(queue.dequeue());
}

TEST_CASE("CommandQueue should override queued commands value if same command is requeued") {
    CommandQueue queue;
    queue.enqueue(CommandType::DumpValve, 50);
    queue.enqueue(CommandType::FillHeatPad, 25);
    queue.enqueue(CommandType::DumpHeatPad, 10);
    queue.enqueue(CommandType::DumpValve, 100);
    queue.enqueue(CommandType::DumpValve, 1);
    queue.enqueue(CommandType::DumpHeatPad, 1000);
    checkCommand(queue.dequeue(), CommandType::DumpValve, 1);
    checkCommand(queue.dequeue(), CommandType::FillHeatPad, 25);
    checkCommand(queue.dequeue(), CommandType::DumpHeatPad, 1000);
    checkNoCommand(queue.dequeue());
}

TEST_CASE("CommandQueue should be able to requeue commands normally after they've been requeued") {
    CommandQueue queue;
    queue.enqueue(CommandType::Reset, 0);
    queue.enqueue(CommandType::DumpHeatPad, 100);
    checkCommand(queue.dequeue(), CommandType::Reset, 0);
    queue.enqueue(CommandType::Reset, 1);
    checkCommand(queue.dequeue(), CommandType::DumpHeatPad, 100);
    checkCommand(queue.dequeue(), CommandType::Reset, 1);
    checkNoCommand(queue.dequeue());
}

TEST_CASE("CommandQueue valve and heat pad commands should not overwrite each other if they target different valves and heatpads") {
    CommandQueue queue;
    queue.enqueue(CommandType::FillHeatPad, 1);
    queue.enqueue(CommandType::DumpHeatPad, 2);
    queue.enqueue(CommandType::NosHeatPad, 3);
    queue.enqueue(CommandType::IpaHeatPad, 4);
    queue.enqueue(CommandType::DumpValve, 5);
    queue.enqueue(CommandType::FillValve, 6);
    checkCommand(queue.dequeue(), CommandType::FillHeatPad, 1);
    checkCommand(queue.dequeue(), CommandType::DumpHeatPad, 2);
    checkCommand(queue.dequeue(), CommandType::NosHeatPad, 3);
    checkCommand(queue.dequeue(), CommandType::IpaHeatPad, 4);
    checkCommand(queue.dequeue(), CommandType::DumpValve, 5);
    checkCommand(queue.dequeue(), CommandType::FillValve, 6);
    checkNoCommand(queue.dequeue());
}

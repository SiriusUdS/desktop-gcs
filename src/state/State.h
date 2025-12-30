#pragma once

#include <atomic>
#include <string>

struct State {
    std::atomic<float> value;
    std::string id;
};

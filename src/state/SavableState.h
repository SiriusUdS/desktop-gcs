#pragma once

#include "State.h"

struct SavableState : public State {
    std::atomic_bool saved;
};

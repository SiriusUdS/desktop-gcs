#pragma once

#include "State.h"

struct SavableStateWidget {
    SavableStateWidget(State& state, std::string label);
    void renderAsRow(float savableValue);

    float readValue{};
    State& state;
    bool saved{};
    std::string label;
    std::string readButtonLabel;
    std::string saveButtonLabel;
    std::string cancelButtonLabel;
};

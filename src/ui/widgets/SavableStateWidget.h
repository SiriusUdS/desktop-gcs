#pragma once

#include "SavableState.h"

struct SavableStateWidget {
    SavableStateWidget(SavableState& state, std::string label);
    void renderAsRow(float savableValue);

    float readValue{};
    SavableState& state;
    std::string label;
    std::string readButtonLabel;
    std::string saveButtonLabel;
    std::string cancelButtonLabel;
};

#pragma once

#include "SessionState.h"

struct SavableSessionState {
    SavableSessionState(SessionState& state, std::string label);
    void renderAsRow(float value);

    float readValue{};
    SessionState& state;
    bool saved{};
    std::string label;
    std::string readButtonLabel;
    std::string saveButtonLabel;
    std::string cancelButtonLabel;
};

#pragma once

#include "UIWindow.h"

struct ValveData;

class ValvesWindow : public UIWindow {
public:
    const char* getName() const override;
    const char* getDockspace() const override;

private:
    void renderImpl() override;
    void renderValveStateRow(const char* label, ValveData& data) const;
};

#pragma once

#include "UIWindow.h"

struct ValveData;

class ValvesWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
    void renderValveStateRow(const char* label, ValveData& data) const;
};

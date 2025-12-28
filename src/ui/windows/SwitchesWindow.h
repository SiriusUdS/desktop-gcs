#pragma once

#include "UIWindow.h"

struct SwitchData;

class SwitchesWindow : public UIWindow {
public:
    const char* getName() const override;
    const char* getDockspace() const override;

private:
    void renderImpl() override;
    void renderSwitch(const SwitchData& data, const char* name) const;
};

#pragma once

#include "UIWindow.h"

struct SwitchData;

class SwitchesWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
    void renderSwitch(const SwitchData& data, const char* name) const;
};

#pragma once

#include "UIWindow.h"

class MonitoringWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
};

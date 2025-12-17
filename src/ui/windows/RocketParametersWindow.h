#pragma once

#include "UIWindow.h"

class RocketParametersWindow : public UIWindow {
public:
    RocketParametersWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    float NOSTankVolume_m3;
};

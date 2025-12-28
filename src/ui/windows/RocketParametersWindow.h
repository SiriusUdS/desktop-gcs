#pragma once

#include "UIWindow.h"

class RocketParametersWindow : public UIWindow {
public:
    RocketParametersWindow();
    const char* getName() const override;
    const char* getDockspace() const override;

private:
    void renderImpl() override;

    float NOSTankVolume_m3;
};

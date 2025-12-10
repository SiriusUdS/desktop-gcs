#pragma once

#include "UIWindow.h"

class RocketParametersWindow : public UIWindow {
public:
    void renderImpl() override;
    const char* name() const override;
    const char* dockspace() const override;
};

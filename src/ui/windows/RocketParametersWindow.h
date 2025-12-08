#ifndef ROCKETPARAMETERSWINDOW_H
#define ROCKETPARAMETERSWINDOW_H

#include "UIWindow.h"

class RocketParametersWindow : public UIWindow {
public:
    void renderImpl() override;
    const char* name() const override;
    const char* dockspace() const override;
};

#endif // ROCKETPARAMETERSWINDOW_H

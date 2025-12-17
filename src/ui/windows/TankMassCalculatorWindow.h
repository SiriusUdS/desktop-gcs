#pragma once

#include "UIWindow.h"

class TankMassCalculatorWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    double tankTemperature_C{};
    double tankPressure_psi{};
};

#pragma once

#include "UIWindow.h"

class TankMassCalculatorWindow : public UIWindow {
public:
    const char* getName() const override;
    const char* getDockspace() const override;

private:
    void renderImpl() override;

    double tankTemperature_C{};
    double tankPressure_psi{};
};

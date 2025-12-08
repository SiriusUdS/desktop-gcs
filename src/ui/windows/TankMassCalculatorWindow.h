#ifndef TANKMASSCALCULATORWINDOW_H
#define TANKMASSCALCULATORWINDOW_H

#include "UIWindow.h"

class TankMassCalculatorWindow : public UIWindow {
public:
    void renderImpl() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    double tankTemperature_C{};
    double tankPressure_psi{};
};

#endif // TANKMASSCALCULATORWINDOW_H

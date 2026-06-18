#pragma once

#include "PlotLine.h"
#include "UIWindow.h"

class TankMassWindow : public UIWindow {
public:
    TankMassWindow();
    void init() override;
    const char* getName() const override;

private:
    void renderImpl() override;

    static const size_t RECENT_TIME_WINDOW_MS = 60'000;

    // TODO: Make these static const
    std::string tankPressurePlotTitle;
    std::string tankTemperaturePlotTitle;
    std::string engineThrustPlotTitle;
    std::string tankMassPlotTitle;

    PlotLine recentMotorPressureSensor1;
    PlotLine recentMotorPressureSensor2;
    PlotLine recentMotorPressureSensor3;
    PlotLine recentFillPressureSensor1;
    PlotLine recentFillPressureSensor2;
    PlotLine recentTankTemperature;
    PlotLine recentEngineThrust;
    PlotLine recentTankMass;
    PlotLine recentTankLoadCell;
};

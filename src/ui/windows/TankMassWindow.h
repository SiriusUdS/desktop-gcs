#pragma once

#include "RecentPlotDataView.h"
#include "UIWindow.h"

class TankMassWindow : public UIWindow {
public:
    TankMassWindow();
    void init() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    static const size_t RECENT_TIME_WINDOW_MS = 60'000;

    // TODO: Make these static const
    std::string tankPressurePlotTitle;
    std::string tankTemperaturePlotTitle;
    std::string engineThrustPlotTitle;
    std::string tankMassPlotTitle;

    // TODO: Replace these indexes with constants
    RecentPlotDataView recentMotorPressureSensor1;
    RecentPlotDataView recentMotorPressureSensor2;
    RecentPlotDataView recentFillPressureSensor1;
    RecentPlotDataView recentFillPressureSensor2;
    RecentPlotDataView recentTankTemperature;
    RecentPlotDataView recentEngineThrust;
    RecentPlotDataView recentTankMass;
    RecentPlotDataView recentTankLoadCell;
};

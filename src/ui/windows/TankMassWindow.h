#ifndef TANKMASSWINDOW_H
#define TANKMASSWINDOW_H

#include "GSDataCenter.h"
#include "RecentPlotData.h"
#include "SensorPlotData.h"
#include "ThemedColors.h"
#include "UIWindow.h"

class TankMassWindow : public UIWindow {
public:
    void init() override;
    void render() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    // TODO: Put this in string utils
    std::string getRecentPlotTitle(std::string title, size_t seconds);

    static const size_t RECENT_TIME_WINDOW_SEC = 60;

    // TODO: Make these static const
    std::string tankPressurePlotTitle;
    std::string tankTemperaturePlotTitle;
    std::string engineThrustPlotTitle;
    std::string tankMassPlotTitle;

    // TODO: Replace these indexes with constants
    RecentPlotData recentMotorPressureSensor1{GSDataCenter::PressureSensor_Motor_PlotData[0].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
    RecentPlotData recentMotorPressureSensor2{GSDataCenter::PressureSensor_Motor_PlotData[1].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
    RecentPlotData recentFillPressureSensor1{GSDataCenter::PressureSensor_FillingStation_PlotData[0].getValuePlotData(),
                                             RECENT_TIME_WINDOW_SEC * 1000,
                                             ThemedColors::GREEN_PLOT_LINE};
    RecentPlotData recentFillPressureSensor2{GSDataCenter::PressureSensor_FillingStation_PlotData[1].getValuePlotData(),
                                             RECENT_TIME_WINDOW_SEC * 1000,
                                             ThemedColors::YELLOW_PLOT_LINE};
    RecentPlotData recentTankTemperature{GSDataCenter::Thermistor_Motor_PlotData[2].getValuePlotData(),
                                         RECENT_TIME_WINDOW_SEC * 1000,
                                         ThemedColors::BLUE_PLOT_LINE};
    RecentPlotData recentEngineThrust{GSDataCenter::LoadCell_FillingStation_PlotData[0].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
    RecentPlotData recentTankMass{GSDataCenter::NOSTankMass_PlotData, RECENT_TIME_WINDOW_SEC * 1000};
    RecentPlotData recentTankLoadCell{GSDataCenter::LoadCell_FillingStation_PlotData[1].getValuePlotData(), RECENT_TIME_WINDOW_SEC * 1000};
};

#endif // TANKMASSWINDOW_H

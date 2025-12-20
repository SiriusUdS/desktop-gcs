#pragma once

#include "GSDataCenterConfig.h"
#include "PlotLine.h"
#include "UIWindow.h"

#include <array>

class MonitoringWindow : public UIWindow {
public:
    MonitoringWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    PlotLine thrustLoadCellPlotLine;

    std::array<PlotLine, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> motorThermistorPlotLines;
};

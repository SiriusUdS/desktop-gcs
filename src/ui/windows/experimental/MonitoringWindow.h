#pragma once

#include "GSDataCenterConfig.h"
#include "PlotLine.h"
#include "UIWindow.h"

#include <array>

class MonitoringWindow : public UIWindow {
public:
    MonitoringWindow();
    const char* getName() const override;
    const char* getDockspace() const override;

    static const char* const name;

private:
    void renderImpl() override;

    PlotLine thrustLoadCellPlotLine;

    std::array<PlotLine, GSDataCenterConfig::THERMISTOR_AMOUNT_PER_BOARD> motorThermistorPlotLines;
};

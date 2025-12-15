#pragma once

#include "CommandTypes.h"
#include "PlotLine.h"
#include "UIWindow.h"

class FillWindow : public UIWindow {
public:
    FillWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    struct PercentageInput {
        int openedValue_perc{};
        int lastSetOpenedValue_perc{};
        int lastOpenedValue_perc{};
    };

    PlotLine tankLoadCellPlotLine;
    PlotLine tankTransducerPlotLine;
    PlotLine tankTempPlotLine;
    PlotLine tankMassPlotLine;

    PercentageInput nosValveSlider;
    PercentageInput ipaValveSlider;
    PercentageInput fillValveSlider;
    PercentageInput dumpValveSlider;
    PercentageInput nosHeatPadSlider;
    PercentageInput ipaHeatPadSlider;
    PercentageInput fillHeatPadSlider;
    PercentageInput dumpHeatPadSlider;

    void renderImpl() override;

    void renderPercentageInputRow(const char* name,
                                  PercentageInput& input,
                                  CommandType commandType,
                                  const char* tooltipDisabled = "",
                                  bool inputEnabled = true) const;
    void addDisabledTooltip(const char* tooltipDisabled, bool inputEnabled) const;
};

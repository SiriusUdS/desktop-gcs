#pragma once

#include "CommandTypes.h"
#include "PlotLine.h"
#include "UIWindow.h"
#include <initializer_list>

class FillWindow : public UIWindow {
public:
    FillWindow();
    const char* getName() const override;

    static const char* const name;

private:
    struct PercentageInput {
        int openedValue_perc{};
        int lastSetOpenedValue_perc{};
        int lastOpenedValue_perc{};
    };

    void renderImpl() override;
    void renderPercentageInputRow(const char* name,
                                  PercentageInput& input,
                                  CommandType commandType,
                                  std::initializer_list<uint32_t> presets = {},
                                  const char* tooltipDisabled = "",
                                  bool inputEnabled = true) const;
    
    void addDisabledTooltip(const char* tooltipDisabled, bool inputEnabled) const;

    PlotLine tankLoadCellPlotLine;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankTransducerPlotLine;
    PlotLine tankTransducerADCPlotLine;
    PlotLine tankTempPlotLine;
    PlotLine tankTempADCPlotLine;
    PlotLine tankMassPlotLine;

    PercentageInput fillValveSlider;
    PercentageInput dumpValveSlider;
    PercentageInput nosHeatPadSlider;
    PercentageInput ipaHeatPadSlider;
    PercentageInput fillHeatPadSlider;
    PercentageInput dumpHeatPadSlider;
};

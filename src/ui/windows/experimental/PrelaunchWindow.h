#pragma once

#include "PlotLine.h"
#include "SavableStateWidget.h"
#include "UIWindow.h"

class PrelaunchWindow : public UIWindow {
public:
    PrelaunchWindow();
    const char* getName() const override;
    const char* getDockspace() const override;

    static const char* const name;

private:
    void renderImpl() override;

    SavableStateWidget postNOSTankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

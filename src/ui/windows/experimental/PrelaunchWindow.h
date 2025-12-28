#pragma once

#include "PlotLine.h"
#include "SavableSessionState.h"
#include "UIWindow.h"

class PrelaunchWindow : public UIWindow {
public:
    PrelaunchWindow();
    const char* getName() const override;
    const char* getDockspace() const override;

    static const char* const name;

private:
    void renderImpl() override;

    SavableSessionState postNOSTankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

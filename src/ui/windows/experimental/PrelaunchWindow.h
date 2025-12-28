#pragma once

#include "PlotLine.h"
#include "SavableSessionState.h"
#include "UIWindow.h"

class PrelaunchWindow : public UIWindow {
public:
    PrelaunchWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    SavableSessionState postNOSTankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

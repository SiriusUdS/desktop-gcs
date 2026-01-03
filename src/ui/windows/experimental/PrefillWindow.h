#pragma once

#include "PlotLine.h"
#include "SavableStateWidget.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* getName() const override;

    static const char* const name;

private:
    void renderImpl() override;

    SensorTestSequencer sensorTestSequencer;
    SavableStateWidget prewrapTankLoadCellState;
    SavableStateWidget postwrapTankLoadCellState;
    SavableStateWidget postIPATankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

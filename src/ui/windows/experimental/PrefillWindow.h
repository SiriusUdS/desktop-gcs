#pragma once

#include "PlotLine.h"
#include "SavableSessionState.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    SensorTestSequencer sensorTestSequencer;
    SavableSessionState prewrapTankLoadCellState;
    SavableSessionState postwrapTankLoadCellState;
    SavableSessionState postIPATankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

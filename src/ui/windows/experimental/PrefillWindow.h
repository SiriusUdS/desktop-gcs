#pragma once

#include "PlotLine.h"
#include "SavableSessionState.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* getName() const override;
    const char* getDockspace() const override;

    static const char* const name;

private:
    void renderImpl() override;

    SensorTestSequencer sensorTestSequencer;
    SavableSessionState prewrapTankLoadCellState;
    SavableSessionState postwrapTankLoadCellState;
    SavableSessionState postIPATankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

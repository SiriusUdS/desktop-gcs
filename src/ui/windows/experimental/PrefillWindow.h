#pragma once

#include "PlotLine.h"
#include "SavableStateWidget.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"
#include <imgui.h>

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* getName() const override;

    static const char* const name;

private:
    void renderImpl() override;
    void renderIndicator(const char* name, bool high, ImColor color_low = ImColor::HSV(0, 0.7f, 0.66f), ImColor color_high = ImColor::HSV(0.29f, 0.7f, 0.66f), const char* label_low = "Low", const char* label_high = "High");

    SensorTestSequencer sensorTestSequencer;
    SavableStateWidget prewrapTankLoadCellState;
    SavableStateWidget postwrapTankLoadCellState;
    SavableStateWidget postIPATankLoadCellState;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

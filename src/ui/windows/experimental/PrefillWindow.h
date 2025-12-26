#pragma once

#include "PlotLine.h"
#include "SensorTestSequencer.h"
#include "SessionState.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    struct TankLoadCellParam {
        TankLoadCellParam(SessionState& state, std::string label);

        float readValue{};
        SessionState& state;
        bool saved{};
        std::string label;
        std::string readButtonLabel;
        std::string saveButtonLabel;
        std::string cancelButtonLabel;
    };

    void renderImpl() override;
    void renderTankLoadCellParam(TankLoadCellParam& tankLoadCellParam);

    SensorTestSequencer sensorTestSequencer;
    TankLoadCellParam prewrapTankLoadCellParam;
    TankLoadCellParam postwrapTankLoadCellParam;
    TankLoadCellParam postIPATankLoadCellParam;
    PlotLine tankLoadCellADCPlotLine;
    PlotLine tankLoadCellPlotLine;
};

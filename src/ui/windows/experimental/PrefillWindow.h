#pragma once

#include "ConfigParam.h"
#include "PlotLine.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    PrefillWindow();
    const char* name() const override;
    const char* dockspace() const override;

private:
    struct TankLoadCellParam {
        TankLoadCellParam(FloatConfigParam& param, std::string label);

        float readValue{};
        FloatConfigParam& param;
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

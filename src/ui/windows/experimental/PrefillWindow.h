#pragma once

#include "Params.h"
#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    struct TankLoadCellParam {
        TankLoadCellParam(FloatParam& param, std::string label);

        FloatParam& param;
        bool saved{};
        std::string label;
        std::string saveButtonLabel;
        std::string cancelButtonLabel;
    };

    void renderImpl() override;

    void renderTankLoadCellParam(TankLoadCellParam& tankLoadCellParam);

    TankLoadCellParam prewrapTankLoadCellParam{Params::TankLoadCell::prewrapADCValue, "Tank Load Cell ADC Value - Prewrap"};
    TankLoadCellParam postwrapTankLoadCellParam{Params::TankLoadCell::postwrapADCValue, "Tank Load Cell ADC Value - Postwrap"};
    TankLoadCellParam postIPATankLoadCellParam{Params::TankLoadCell::postIPAADCValue, "Tank Load Cell ADC Value - Post IPA"};
    SensorTestSequencer sensorTestSequencer;
};

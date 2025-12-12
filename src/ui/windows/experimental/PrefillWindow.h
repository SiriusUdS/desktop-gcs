#pragma once

#include "SensorTestSequencer.h"
#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    float tankLoadCellADCValue{};
    float tankLoadCellADCValuePostWrap{};
    float tankLoadCellADCValuePostIPA{};

    bool tankLoadCellADCValueSaved{};
    bool tankLoadCellADCValuePostWrapSaved{};
    bool tankLoadCellADCValuePostIPASaved{};
    SensorTestSequencer sensorTestSequencer;
};

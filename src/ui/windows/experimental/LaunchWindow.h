#pragma once

#include "StateMachineRenderer.h"
#include "UIWindow.h"

class LaunchWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void lazyInit() override;
    void renderImpl() override;

    void initFillStationStateMachine();
    void initMotorStateMachine();

    StateMachineRenderer fsStateMachine;
    StateMachineRenderer motorStateMachine;
};

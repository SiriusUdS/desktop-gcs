#pragma once

#include "StateMachineRenderer.h"
#include "UIWindow.h"

class LaunchWindow : public UIWindow {
public:
    void lazyInit() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;

    void initFillStationStateMachine();
    void initMotorStateMachine();

    StateMachineRenderer fsStateMachine;
    StateMachineRenderer motorStateMachine;
};

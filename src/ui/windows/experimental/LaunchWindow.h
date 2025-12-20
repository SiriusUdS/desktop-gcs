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

    bool fsInitStateActive{true};
    bool fsSafeStateActive{};
    bool fsTestStateActive{};
    bool fsAbortStateActive{};
    bool fsErrorStateActive{};
    bool fsUnsafeStateActive{};
    bool fsIgniteStateActive{};

    bool motorInitStateActive{true};
    bool motorSafeStateActive{};
    bool motorTestStateActive{};
    bool motorAbortStateActive{};
    bool motorErrorStateActive{};
    bool motorUnsafeStateActive{};
    bool motorIgniteStateActive{};
    bool motorFireStateActive{};

    bool inLaunchState{};

    StateMachineRenderer fsStateMachine;
    StateMachineRenderer motorStateMachine;
};

#ifndef LAUNCHWINDOW_H
#define LAUNCHWINDOW_H

#include "StateMachineRenderer.h"
#include "UIWindow.h"

class LaunchWindow : public UIWindow {
public:
    LaunchWindow();
    void render() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    StateMachineRenderer fsStateMachine; // Filling station state machine
};

#endif // LAUNCHWINDOW_H

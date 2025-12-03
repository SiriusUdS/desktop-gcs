#ifndef SWITCHESWINDOW_H
#define SWITCHESWINDOW_H

#include "UIWindow.h"

struct SwitchData;

class SwitchesWindow : public UIWindow {
public:
    void render() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderSwitch(const SwitchData& data, const char* name) const;
};

#endif // SWITCHESWINDOW_H

#ifndef VALVESWINDOW_H
#define VALVESWINDOW_H

#include "UIWindow.h"

struct ValveData;

class ValvesWindow : public UIWindow {
public:
    void renderImpl() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderValveStateRow(const char* label, ValveData& data) const;
};

#endif // VALVESWINDOW_H

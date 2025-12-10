#ifndef PREFILLWINDOW_H
#define PREFILLWINDOW_H

#include "UIWindow.h"

class PrefillWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
};

#endif // PREFILLWINDOW_H

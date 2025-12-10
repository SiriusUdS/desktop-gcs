#ifndef FILLWINDOW_H
#define FILLWINDOW_H

#include "UIWindow.h"

class FillWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
};

#endif // FILLWINDOW_H

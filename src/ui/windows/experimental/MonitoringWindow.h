#ifndef MONITORINGWINDOW_H
#define MONITORINGWINDOW_H

#include "UIWindow.h"

class MonitoringWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
};

#endif // MONITORINGWINDOW_H

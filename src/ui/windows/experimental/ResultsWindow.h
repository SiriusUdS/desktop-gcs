#ifndef RESULTSWINDOW_H
#define RESULTSWINDOW_H

#include "UIWindow.h"

class ResultsWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
};

#endif // RESULTSWINDOW_H

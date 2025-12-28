#pragma once

#include "UIWindow.h"

class ResultsWindow : public UIWindow {
public:
    const char* getName() const override;
    const char* getDockspace() const override;

    static const char* const name;

private:
    void renderImpl() override;
};

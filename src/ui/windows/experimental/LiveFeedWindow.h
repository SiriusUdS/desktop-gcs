#pragma once

#include "UIWindow.h"

class LiveFeedWindow : public UIWindow {
public:
    const char* getName() const override;

private:
    void renderImpl() override;
};

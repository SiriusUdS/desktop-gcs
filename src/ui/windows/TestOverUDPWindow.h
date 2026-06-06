#pragma once
#include "UIWindow.h"

class TestOverUDPWindow:public UIWindow {
public:
    TestOverUDPWindow();
    
    const char* getName() const override;
    
    void renderImpl() override;
};

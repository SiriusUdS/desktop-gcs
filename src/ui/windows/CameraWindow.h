#pragma once
#include "CameraManager.h"
#include "UIWindow.h"

class CameraWindow: public UIWindow {
public:
    CameraWindow();
    
    const char* getName() const override;
    
    void renderImpl() override;
};

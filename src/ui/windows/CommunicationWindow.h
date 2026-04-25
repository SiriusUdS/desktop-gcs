#pragma once
#include "UIWindow.h"

class CommunicationWindow: public UIWindow {
public:
    const char* getName() const override;
    
    static const char* const name;
private:
    void renderImpl() override;
};

#pragma once

#include "BoardComStateMonitor.h"
#include "UIWindow.h"

class BoardsWindow : public UIWindow {
public:
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderImpl() override;
    void renderBoardTableRow(const char* name, const char* boardStateName, BoardComStateMonitor::State comState) const;
    void renderStorageErrorStatusName(uint16_t storageErrorStatus) const;
};

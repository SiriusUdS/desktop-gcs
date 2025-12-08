#ifndef BOARDSWINDOW_H
#define BOARDSWINDOW_H

#include "BoardComStateMonitor.h"
#include "UIWindow.h"

class BoardsWindow : public UIWindow {
public:
    void renderImpl() override;
    const char* name() const override;
    const char* dockspace() const override;

private:
    void renderBoardTableRow(const char* name, const char* boardStateName, BoardComStateMonitor::State comState) const;
    void renderStorageErrorStatusName(uint16_t storageErrorStatus) const;
};

#endif // BOARDSWINDOW_H

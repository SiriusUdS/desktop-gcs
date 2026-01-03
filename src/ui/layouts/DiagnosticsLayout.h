#pragma once

#include "Layout.h"

class DiagnosticsLayout : public Layout {
private:
    const char* getName() override;
    std::vector<HelloImGui::DockingSplit> createDockingSplits() override;
    std::vector<DockedWindow> getDockedWindows() override;

    static const char* MAIN_DOCKSPACE;
    static const char* PLOT_DOCKSPACE;
    static const char* LOG_DOCKSPACE;
};

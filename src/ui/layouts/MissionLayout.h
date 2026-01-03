#pragma once

#include "Layout.h"

class MissionLayout : public Layout {
private:
    const char* getName() override;
    std::vector<HelloImGui::DockingSplit> createDockingSplits() override;
    std::vector<DockedWindow> getDockedWindows() override;

    static const char* MISSION_DOCKSPACE;
    static const char* LIVE_FEED_DOCKSPACE;
};

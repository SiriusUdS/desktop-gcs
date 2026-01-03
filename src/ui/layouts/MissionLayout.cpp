#include "MissionLayout.h"

#include "UIWindows.h"

const char* MissionLayout::MISSION_DOCKSPACE = Layout::MAIN_DOCKSPACE;
const char* MissionLayout::LIVE_FEED_DOCKSPACE = "LiveFeedLayout";

const char* MissionLayout::getName() {
    return "Mission";
}

std::vector<HelloImGui::DockingSplit> MissionLayout::createDockingSplits() {
    HelloImGui::DockingSplit verticalSplit;
    verticalSplit.initialDock = MissionLayout::MISSION_DOCKSPACE;
    verticalSplit.newDock = MissionLayout::LIVE_FEED_DOCKSPACE;
    verticalSplit.direction = ImGuiDir_Right;
    verticalSplit.ratio = 0.5f;

    return {verticalSplit};
}

std::vector<Layout::DockedWindow> MissionLayout::getDockedWindows() {
    std::vector<Layout::DockedWindow> dockableWindows;

    dockableWindows.emplace_back(UIWindows::prefillWindow, MISSION_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::fillWindow, MISSION_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::prelaunchWindow, MISSION_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::launchWindow, MISSION_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::monitoringWindow, MISSION_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::resultsWindow, MISSION_DOCKSPACE);

    dockableWindows.emplace_back(UIWindows::liveFeedWindow, LIVE_FEED_DOCKSPACE);

    return dockableWindows;
}

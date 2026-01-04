#include "MissionLayout.h"

#include "UIWindows.h"

const char* MissionLayout::MISSION_DOCKSPACE = Layout::MAIN_DOCKSPACE;
const char* MissionLayout::LIVE_FEED_DOCKSPACE = "LiveFeedLayout";
const char* MissionLayout::LOG_DOCKSPACE = "LogDockSpace";

const char* MissionLayout::getName() {
    return "Mission";
}

std::vector<HelloImGui::DockingSplit> MissionLayout::createDockingSplits() {
    HelloImGui::DockingSplit logSplit;
    logSplit.initialDock = MAIN_DOCKSPACE;
    logSplit.newDock = LOG_DOCKSPACE;
    logSplit.direction = ImGuiDir_Down;
    logSplit.ratio = 0.33f;

    HelloImGui::DockingSplit liveFeedSplit;
    liveFeedSplit.initialDock = MissionLayout::MISSION_DOCKSPACE;
    liveFeedSplit.newDock = MissionLayout::LIVE_FEED_DOCKSPACE;
    liveFeedSplit.direction = ImGuiDir_Right;
    liveFeedSplit.ratio = 0.5f;

    return {logSplit, liveFeedSplit};
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

    dockableWindows.emplace_back(UIWindows::loggingWindow, LOG_DOCKSPACE);

    return dockableWindows;
}

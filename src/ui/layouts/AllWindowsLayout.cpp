#include "AllWindowsLayout.h"

#include "UIWindows.h"

const char* AllWindowsLayout::MAIN_DOCKSPACE = Layout::MAIN_DOCKSPACE;
const char* AllWindowsLayout::PLOT_DOCKSPACE = "PlotDockSpace";
const char* AllWindowsLayout::LOG_DOCKSPACE = "LogDockSpace";

const char* AllWindowsLayout::getName() {
    return "All Windows";
}

std::vector<HelloImGui::DockingSplit> AllWindowsLayout::createDockingSplits() {
    HelloImGui::DockingSplit logSplit;
    logSplit.initialDock = MAIN_DOCKSPACE;
    logSplit.newDock = LOG_DOCKSPACE;
    logSplit.direction = ImGuiDir_Down;
    logSplit.ratio = 0.33f;

    HelloImGui::DockingSplit plotSplit;
    plotSplit.initialDock = MAIN_DOCKSPACE;
    plotSplit.newDock = PLOT_DOCKSPACE;
    plotSplit.direction = ImGuiDir_Right;
    plotSplit.ratio = 0.5f;

    return {logSplit, plotSplit};
}

std::vector<Layout::DockedWindow> AllWindowsLayout::getDockedWindows() {
    std::vector<Layout::DockedWindow> dockableWindows;

    dockableWindows.emplace_back(UIWindows::boardsWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::mapWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::serialComWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::switchesWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::valvesWindow, MAIN_DOCKSPACE);

    dockableWindows.emplace_back(UIWindows::prefillWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::fillWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::prelaunchWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::launchWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::monitoringWindow, MAIN_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::resultsWindow, MAIN_DOCKSPACE);

    dockableWindows.emplace_back(UIWindows::nosPhaseDiagramWindow, PLOT_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::tankMassWindow, PLOT_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::liveFeedWindow, PLOT_DOCKSPACE);

    dockableWindows.emplace_back(UIWindows::loggingWindow, LOG_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::rocketParametersWindow, LOG_DOCKSPACE);
    dockableWindows.emplace_back(UIWindows::tankMassCalculatorWindow, LOG_DOCKSPACE);

    return dockableWindows;
}

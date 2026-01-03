#include "UIWindows.h"

namespace UIWindows {
std::shared_ptr<BoardsWindow> boardsWindow = std::make_shared<BoardsWindow>();
std::shared_ptr<ControlsWindow> controlsWindow = std::make_shared<ControlsWindow>();
std::shared_ptr<FillWindow> fillWindow = std::make_shared<FillWindow>();
std::shared_ptr<LaunchWindow> launchWindow = std::make_shared<LaunchWindow>();
std::shared_ptr<LiveFeedWindow> liveFeedWindow = std::make_shared<LiveFeedWindow>();
std::shared_ptr<LoggingWindow> loggingWindow = std::make_shared<LoggingWindow>();
std::shared_ptr<MapWindow> mapWindow = std::make_shared<MapWindow>();
std::shared_ptr<MonitoringWindow> monitoringWindow = std::make_shared<MonitoringWindow>();
std::shared_ptr<NOSPhaseDiagramWindow> nosPhaseDiagramWindow = std::make_shared<NOSPhaseDiagramWindow>();
std::shared_ptr<PrefillWindow> prefillWindow = std::make_shared<PrefillWindow>();
std::shared_ptr<PrelaunchWindow> prelaunchWindow = std::make_shared<PrelaunchWindow>();
std::shared_ptr<ResultsWindow> resultsWindow = std::make_shared<ResultsWindow>();
std::shared_ptr<RocketParametersWindow> rocketParametersWindow = std::make_shared<RocketParametersWindow>();
std::shared_ptr<SerialComWindow> serialComWindow = std::make_shared<SerialComWindow>();
std::shared_ptr<SwitchesWindow> switchesWindow = std::make_shared<SwitchesWindow>();
std::shared_ptr<TankMassCalculatorWindow> tankMassCalculatorWindow = std::make_shared<TankMassCalculatorWindow>();
std::shared_ptr<TankMassWindow> tankMassWindow = std::make_shared<TankMassWindow>();
std::shared_ptr<ValvesWindow> valvesWindow = std::make_shared<ValvesWindow>();

std::vector<std::shared_ptr<UIWindow>> windows = {boardsWindow,
                                                  controlsWindow,
                                                  fillWindow,
                                                  launchWindow,
                                                  liveFeedWindow,
                                                  loggingWindow,
                                                  mapWindow,
                                                  monitoringWindow,
                                                  nosPhaseDiagramWindow,
                                                  prefillWindow,
                                                  prelaunchWindow,
                                                  resultsWindow,
                                                  rocketParametersWindow,
                                                  serialComWindow,
                                                  switchesWindow,
                                                  tankMassCalculatorWindow,
                                                  tankMassWindow,
                                                  valvesWindow};
} // namespace UIWindows

void UIWindows::init() {
    for (auto& window : windows) {
        window->init();
    }
}

void UIWindows::loadState(const mINI::INIStructure& ini) {
    for (auto& window : windows) {
        window->loadState(ini);
    }
}

void UIWindows::saveState(mINI::INIStructure& ini) {
    for (auto& window : windows) {
        window->saveState(ini);
    }
}

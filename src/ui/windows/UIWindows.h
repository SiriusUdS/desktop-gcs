#pragma once

#include "BoardsWindow.h"
#include "ControlsWindow.h"
#include "FillWindow.h"
#include "LaunchWindow.h"
#include "LiveFeedWindow.h"
#include "LoggingWindow.h"
#include "MapWindow.h"
#include "MonitoringWindow.h"
#include "NOSPhaseDiagramWindow.h"
#include "PrefillWindow.h"
#include "PrelaunchWindow.h"
#include "ResultsWindow.h"
#include "RocketParametersWindow.h"
#include "SerialComWindow.h"
#include "SwitchesWindow.h"
#include "TankMassCalculatorWindow.h"
#include "TankMassWindow.h"
#include "ValvesWindow.h"

#include <memory>

namespace UIWindows {
extern std::shared_ptr<BoardsWindow> boardsWindow;
extern std::shared_ptr<ControlsWindow> controlsWindow;
extern std::shared_ptr<FillWindow> fillWindow;
extern std::shared_ptr<LaunchWindow> launchWindow;
extern std::shared_ptr<LiveFeedWindow> liveFeedWindow;
extern std::shared_ptr<LoggingWindow> loggingWindow;
extern std::shared_ptr<MapWindow> mapWindow;
extern std::shared_ptr<MonitoringWindow> monitoringWindow;
extern std::shared_ptr<NOSPhaseDiagramWindow> nosPhaseDiagramWindow;
extern std::shared_ptr<PrefillWindow> prefillWindow;
extern std::shared_ptr<PrelaunchWindow> prelaunchWindow;
extern std::shared_ptr<ResultsWindow> resultsWindow;
extern std::shared_ptr<RocketParametersWindow> rocketParametersWindow;
extern std::shared_ptr<SerialComWindow> serialComWindow;
extern std::shared_ptr<SwitchesWindow> switchesWindow;
extern std::shared_ptr<TankMassCalculatorWindow> tankMassCalculatorWindow;
extern std::shared_ptr<TankMassWindow> tankMassWindow;
extern std::shared_ptr<ValvesWindow> valvesWindow;

extern std::vector<std::shared_ptr<UIWindow>> windows;

void init();
void loadState(const mINI::INIStructure& ini);
void saveState(mINI::INIStructure& ini);
} // namespace UIWindows

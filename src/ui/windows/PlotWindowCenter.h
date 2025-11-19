#ifndef PLOTWINDOWCENTER_H
#define PLOTWINDOWCENTER_H

#include "PlotWindow.h"

#include <hello_imgui/hello_imgui.h>

namespace PlotWindowCenter {
std::vector<HelloImGui::DockableWindow> createDockableWindows();
void loadState(const mINI::INIStructure& iniStructure);
void saveState(mINI::INIStructure& iniStructure);

extern PlotWindow thermistorMotorPlot;
extern PlotWindow pressureSensorMotorPlot;
extern PlotWindow thermistorFillingStationPlot;
extern PlotWindow pressureSensorFillingStationPlot;
extern PlotWindow loadCellFillingStationPlot;
} // namespace PlotWindowCenter

#endif // PLOTWINDOWCENTER_H

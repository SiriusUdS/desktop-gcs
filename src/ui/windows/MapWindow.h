#ifndef MAPWINDOW_H
#define MAPWINDOW_H

#include "RichMapPlot.h"
#include "RichMarkStorage.h"
#include "TileGrabber.h"
#include "TileSourceUrlConnTest.h"
#include "TileSourceUrlImpl.h"

#include <chrono>

namespace MapWindow {
enum MapView { MAP_VIEW = 0, SATELLITE_VIEW = 1 };

void init();
void loadState(const mINI::INIStructure& ini);
void saveState(mINI::INIStructure& ini);
void render();
} // namespace MapWindow

#endif // MAPWINDOW_H

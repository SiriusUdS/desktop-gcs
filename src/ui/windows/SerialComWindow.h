#ifndef SERIALCOMWINDOW_H
#define SERIALCOMWINDOW_H

#include <ini.h>

namespace SerialComWindow {
void render();
void loadState(const mINI::INIStructure& ini);
void saveState(mINI::INIStructure& ini);
} // namespace SerialComWindow

#endif // SERIALCOMWINDOW_H

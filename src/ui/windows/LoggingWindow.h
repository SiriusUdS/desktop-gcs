#ifndef LOGGINGWINDOW_H
#define LOGGINGWINDOW_H

#include <imgui.h>
#include <ini.h>
#include <spdlog/spdlog.h>

namespace LoggingWindow {
void render();
void loadState(const mINI::INIStructure& ini);
void saveState(mINI::INIStructure& ini);
void addLog(const char* str, const char* strEnd, spdlog::level::level_enum type);
} // namespace LoggingWindow

#endif // LOGGINGWINDOW_H

#pragma once

#include <chrono>
#include <hello_imgui/hello_imgui.h>
#include <ini.h>

class LoggingWindow;

namespace Application {
void loadFonts();
void init();
void preNewFrame();
void shutdown();
std::vector<HelloImGui::DockingSplit> createBaseDockingSplits();
std::vector<HelloImGui::DockableWindow> createDockableWindows();

extern std::shared_ptr<LoggingWindow> loggingWindow;
} // namespace Application

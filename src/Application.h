#pragma once

#include <chrono>
#include <hello_imgui/hello_imgui.h>
#include <ini.h>

class LoggingWindow;

namespace Application {
void loadFonts();
void init();
void preNewFrame();
void showMenus();
void shutdown();
} // namespace Application

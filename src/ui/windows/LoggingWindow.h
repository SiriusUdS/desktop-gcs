#pragma once

#include "LogBuffer.h"
#include "UIWindow.h"

#include <imgui.h>
#include <ini.h>
#include <spdlog/spdlog.h>

class LoggingWindow : public UIWindow {
public:
    LoggingWindow();

    void renderImpl() override;
    void loadState(const mINI::INIStructure& ini) override;
    void saveState(mINI::INIStructure& ini) const override;
    const char* name() const override;
    const char* dockspace() const override;

    void addLog(const char* str, const char* strEnd, spdlog::level::level_enum type);

private:
    const char* GCS_INI_LOG_WINDOW_AUTO_SCROLL = "log_window_auto_scroll";
    const char* GCS_INI_LOG_WINDOW_SHOW_DEBUG = "log_window_show_debug";
    const char* GCS_INI_LOG_WINDOW_SHOW_INFO = "log_window_show_info";
    const char* GCS_INI_LOG_WINDOW_SHOW_WARN = "log_window_show_warn";
    const char* GCS_INI_LOG_WINDOW_SHOW_ERROR = "log_window_show_error";

    bool showInfo{true}, showWarn{true}, showError{true}, showTrace{true}, showDebug{true};
    ImGuiTextFilter filter;
    LogBuffer logBuffer;
};

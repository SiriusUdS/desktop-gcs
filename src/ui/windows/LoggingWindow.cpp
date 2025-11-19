#include "LoggingWindow.h"

#include "FontAwesome.h"
#include "IniConfig.h"
#include "LogBuffer.h"
#include "Logging.h"
#include "ToggleButton.h"
#include "UIConfig.h"

#include <ini.h>
#include <mutex>

namespace LoggingWindow {
void renderLogs();
void clear();

constexpr const char* GCS_INI_LOG_WINDOW_AUTO_SCROLL = "log_window_auto_scroll";
constexpr const char* GCS_INI_LOG_WINDOW_SHOW_DEBUG = "log_window_show_debug";
constexpr const char* GCS_INI_LOG_WINDOW_SHOW_INFO = "log_window_show_info";
constexpr const char* GCS_INI_LOG_WINDOW_SHOW_WARN = "log_window_show_warn";
constexpr const char* GCS_INI_LOG_WINDOW_SHOW_ERROR = "log_window_show_error";

bool showInfo{true}, showWarn{true}, showError{true}, showTrace{true}, showDebug{true};
ImGuiTextFilter filter;
LogBuffer logBuffer(filter, showDebug, showInfo, showWarn, showError);
std::mutex mtx;
} // namespace LoggingWindow

void LoggingWindow::render() {
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
    if (ToggleButton(ICON_FA_BUG " Debug", &showDebug, UIConfig::BLUE_BUTTON_COLOR)) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_INFO_CIRCLE " Info", &showInfo, UIConfig::GREEN_BUTTON_COLOR)) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_EXCLAMATION_TRIANGLE " Warn", &showWarn, UIConfig::YELLOW_BUTTON_COLOR)) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_BOMB " Error", &showError, UIConfig::RED_BUTTON_COLOR)) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        clear();
    }
    ImGui::SameLine();
    if (filter.Draw("Filter", -100.0f)) {
        logBuffer.updateVisibleLogs();
    }

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        renderLogs();
    }
    ImGui::EndChild();
}

void LoggingWindow::loadState(const mINI::INIStructure& ini) {
    if (ini.has(IniConfig::GCS_SECTION)) {
        if (ini.get(IniConfig::GCS_SECTION).has(GCS_INI_LOG_WINDOW_SHOW_DEBUG)) {
            showDebug = std::stoi(ini.get(IniConfig::GCS_SECTION).get(GCS_INI_LOG_WINDOW_SHOW_DEBUG));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(GCS_INI_LOG_WINDOW_SHOW_INFO)) {
            showInfo = std::stoi(ini.get(IniConfig::GCS_SECTION).get(GCS_INI_LOG_WINDOW_SHOW_INFO));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(GCS_INI_LOG_WINDOW_SHOW_WARN)) {
            showWarn = std::stoi(ini.get(IniConfig::GCS_SECTION).get(GCS_INI_LOG_WINDOW_SHOW_WARN));
        }
        if (ini.get(IniConfig::GCS_SECTION).has(GCS_INI_LOG_WINDOW_SHOW_ERROR)) {
            showError = std::stoi(ini.get(IniConfig::GCS_SECTION).get(GCS_INI_LOG_WINDOW_SHOW_ERROR));
        }
    }
}

void LoggingWindow::saveState(mINI::INIStructure& ini) {
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_DEBUG, std::to_string(showDebug));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_INFO, std::to_string(showInfo));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_WARN, std::to_string(showWarn));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_ERROR, std::to_string(showError));
}

void LoggingWindow::addLog(const char* str, const char* strEnd, spdlog::level::level_enum type) {
    std::lock_guard<std::mutex> lock(mtx);
    logBuffer.addLog(str, strEnd, type);
}

void LoggingWindow::renderLogs() {
    std::lock_guard<std::mutex> lock(mtx);
    logBuffer.render();
}

void LoggingWindow::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    logBuffer.clear();
}

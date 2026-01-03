#include "LoggingWindow.h"

#include "FontAwesome.h"
#include "IniConfig.h"
#include "ThemedColors.h"
#include "ToggleButton.h"

#include <ini.h>

LoggingWindow::LoggingWindow() : logBuffer{filter, showDebug, showInfo, showWarn, showError} {
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

void LoggingWindow::saveState(mINI::INIStructure& ini) const {
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_DEBUG, std::to_string(showDebug));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_INFO, std::to_string(showInfo));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_WARN, std::to_string(showWarn));
    ini[IniConfig::GCS_SECTION].set(GCS_INI_LOG_WINDOW_SHOW_ERROR, std::to_string(showError));
}

const char* LoggingWindow::getName() const {
    return "Logs";
}

void LoggingWindow::addLog(const char* str, const char* strEnd, spdlog::level::level_enum type) {
    logBuffer.addLog(str, strEnd, type);
}

void LoggingWindow::renderImpl() {
    if (ToggleButton(ICON_FA_BUG " Debug", &showDebug, ThemedColors::Button::blue.resolve())) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_INFO_CIRCLE " Info", &showInfo, ThemedColors::Button::green.resolve())) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_EXCLAMATION_TRIANGLE " Warn", &showWarn, ThemedColors::Button::yellow.resolve())) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ToggleButton(ICON_FA_BOMB " Error", &showError, ThemedColors::Button::red.resolve())) {
        logBuffer.updateVisibleLogs();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        logBuffer.clear();
    }
    ImGui::SameLine();
    if (filter.Draw("Filter", -100.0f)) {
        logBuffer.updateVisibleLogs();
    }

    ImGui::Separator();

    // TODO: TEXT COLOR IS WRONG WHEN RENDERING LOG MESSAGES
    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        logBuffer.render();
    }
    ImGui::EndChild();
}

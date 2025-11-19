#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include <imgui.h>
#include <spdlog/spdlog.h>

class LogBuffer {
public:
    LogBuffer(const ImGuiTextFilter& filter, const bool& showDebug, const bool& showInfo, const bool& showWarn, const bool& showError);
    void addLog(const char* str, const char* strEnd, spdlog::level::level_enum level);
    void render();
    void clear();
    void updateVisibleLogs();

private:
    bool isLogVisible(const char* lineStart, const char* lineEnd, spdlog::level::level_enum level);

    const ImGuiTextFilter& filter;
    const bool& showDebug;
    const bool& showInfo;
    const bool& showWarn;
    const bool& showError;

    ImGuiTextBuffer buf;
    ImVector<int> lineOffsets;
    ImVector<spdlog::level::level_enum> logLevels;
    ImVector<int> visibleLines;
    std::mutex mtx;
};

#endif // LOGBUFFER_H

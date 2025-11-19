#include "LogBuffer.h"

LogBuffer::LogBuffer(const ImGuiTextFilter& filter, const bool& showDebug, const bool& showInfo, const bool& showWarn, const bool& showError)
    : filter(filter), showDebug(showDebug), showInfo(showInfo), showWarn(showWarn), showError(showError) {
    clear();
}

void LogBuffer::addLog(const char* str, const char* strEnd, spdlog::level::level_enum level) {
    int oldSize = buf.size();
    buf.append(str, strEnd);
    const char* lineStart = buf.begin() + oldSize;

    for (const int newSize = buf.size(); oldSize < newSize; oldSize++) {
        if (buf[oldSize] == '\n') {
            const char* lineEnd = buf.begin() + oldSize;

            if (isLogVisible(lineStart, lineEnd, level)) {
                visibleLines.push_back(lineOffsets.size() - 1);
            }

            lineOffsets.push_back(oldSize + 1);
            logLevels.push_back(level);

            lineStart = lineEnd + 1;
        }
    }
}

void LogBuffer::render() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    const char* bufBegin = buf.begin();
    const char* bufEnd = buf.end();

    ImGuiListClipper clipper;
    clipper.Begin(visibleLines.size());
    while (clipper.Step()) {
        for (int lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; lineNo++) {
            const int lineIdx = visibleLines[lineNo];
            const char* lineStart = bufBegin + lineOffsets[lineIdx];
            const char* lineEnd = (lineIdx + 1 < lineOffsets.size()) ? (bufBegin + lineOffsets[lineIdx + 1]) : bufEnd;
            ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f);

            // clang-format off
            switch (logLevels[lineIdx]) {
                case spdlog::level::debug: color = ImVec4(0.2f, 0.6f, 1.0f, 1.0f); break;
                case spdlog::level::info:  color = ImVec4(0.0f, 0.8f, 0.0f, 1.0f); break;
                case spdlog::level::warn:  color = ImVec4(0.65f, 0.65f, 0.0f, 1.0f); break;
                case spdlog::level::err:   color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
            }
            // clang-format on

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(lineStart, lineEnd);
            ImGui::PopStyleColor();
        }
    }
    clipper.End();

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::PopStyleVar();
}

void LogBuffer::clear() {
    buf.clear();
    lineOffsets.clear();
    lineOffsets.push_back(0);
    logLevels.clear();
    updateVisibleLogs();
}

void LogBuffer::updateVisibleLogs() {
    visibleLines.clear();

    const char* bufBegin = buf.begin();
    const char* bufEnd = buf.end();

    for (int lineNo = 0; lineNo < lineOffsets.size() - 1; lineNo++) {
        const char* lineStart = bufBegin + lineOffsets[lineNo];
        const char* lineEnd = bufBegin + lineOffsets[lineNo + 1];

        if (!isLogVisible(lineStart, lineEnd, logLevels[lineNo])) {
            continue;
        }

        visibleLines.push_back(lineNo);
    }
}

bool LogBuffer::isLogVisible(const char* lineStart, const char* lineEnd, spdlog::level::level_enum level) {
    bool showLevel = true;

    // clang-format off
    switch (level) {
        case spdlog::level::debug: showLevel = showDebug; break;
        case spdlog::level::info:  showLevel = showInfo;  break;
        case spdlog::level::warn:  showLevel = showWarn;  break;
        case spdlog::level::err:   showLevel = showError; break;
    }
    // clang-format on

    return showLevel && (!filter.IsActive() || filter.PassFilter(lineStart, lineEnd));
}

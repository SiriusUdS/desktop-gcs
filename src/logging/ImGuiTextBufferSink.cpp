#include "ImGuiTextBufferSink.h"

#include "Application.h"
#include "LoggingWindow.h"

/**
 * @brief Add log to an ImGui text buffer
 */
void ImGuiTextBufferSink::sink_it_(const spdlog::details::log_msg& msg) {
    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);
    Application::loggingWindow->addLog(formatted.data(), formatted.data() + formatted.size(), msg.level);
}

/**
 * @brief Implementation of the flush_ pure virtual function that does nothing
 */
void ImGuiTextBufferSink::flush_() {
    // ImGuiTextBuffer directly appends data to an internal buffer and does not need a flush function
};

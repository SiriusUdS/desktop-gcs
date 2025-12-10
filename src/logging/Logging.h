#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Logging {
void init();

inline std::shared_ptr<spdlog::logger> appLogger{};
inline std::mutex mtx;

template <class... Args>
void implAppDebugLog(fmt::format_string<Args...> fmt, Args&&... args) {
    std::lock_guard<std::mutex> lock(mtx);
    appLogger->debug(fmt, std::forward<Args>(args)...);
}

template <class... Args>
void implAppErrorLog(fmt::format_string<Args...> fmt, Args&&... args) {
    std::lock_guard<std::mutex> lock(mtx);
    appLogger->error(fmt, std::forward<Args>(args)...);
}

template <class... Args>
void implAppInfoLog(fmt::format_string<Args...> fmt, Args&&... args) {
    std::lock_guard<std::mutex> lock(mtx);
    appLogger->info(fmt, std::forward<Args>(args)...);
}

template <class... Args>
void implAppWarnLog(fmt::format_string<Args...> fmt, Args&&... args) {
    std::lock_guard<std::mutex> lock(mtx);
    appLogger->warn(fmt, std::forward<Args>(args)...);
}
} // namespace Logging

#define GCS_APP_LOG_DEBUG(fmt, ...) Logging::implAppDebugLog(fmt, __VA_ARGS__)
#define GCS_APP_LOG_ERROR(fmt, ...) Logging::implAppErrorLog(fmt, __VA_ARGS__)
#define GCS_APP_LOG_INFO(fmt, ...) Logging::implAppInfoLog(fmt, __VA_ARGS__)
#define GCS_APP_LOG_WARN(fmt, ...) Logging::implAppWarnLog(fmt, __VA_ARGS__)

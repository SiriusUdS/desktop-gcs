#pragma once

// Precompiled header — parse the heavy, pervasive third-party and standard
// headers once instead of in every translation unit. Force-included into every
// TU via premake (forceincludes), so individual sources need not include it.

#ifdef _WIN32
// Keep windows.h (pulled in transitively by spdlog / hello_imgui) from dragging
// in the legacy winsock.h, which would clash with <WinSock2.h> in the com layer.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

// Heavy third-party UI / logging headers — the dominant per-TU parse cost.
#include <imgui.h>
#include <implot.h>
#include <hello_imgui/hello_imgui.h>
#include <spdlog/spdlog.h>

// Common standard-library headers used throughout the codebase.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

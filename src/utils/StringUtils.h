#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>

namespace StringUtils {
void convertStringToIniId(std::string& s);
std::string getRecentPlotLineTitle(std::string title, size_t seconds);

#ifdef _WIN32
std::string wcharToString(const WCHAR* wstr);
#endif
} // namespace StringUtils

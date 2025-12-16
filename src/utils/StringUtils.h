#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <string>

namespace StringUtils {
void convertStringToIniId(std::string& s);

#ifdef _WIN32
std::string wcharToString(const WCHAR* wstr);
#endif
} // namespace StringUtils

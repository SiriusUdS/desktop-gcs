#include "StringUtils.h"

#include <vector>

void StringUtils::convertStringToIniId(std::string& s) {
    for (auto& c : s) {
        if (c == ' ') {
            c = '_';
        } else {
            c = std::tolower(c);
        }
    }
}

#ifdef _WIN32
std::string StringUtils::wcharToString(const WCHAR* wstr) {
    if (wstr == nullptr)
        return {};

    // Get the length of the resulting string
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (size_needed == 0)
        return {};

    std::vector<char> buffer(size_needed);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, buffer.data(), size_needed, nullptr, nullptr);

    return std::string(buffer.data());
}
#endif

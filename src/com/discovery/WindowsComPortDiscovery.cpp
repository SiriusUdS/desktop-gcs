#include "WindowsComPortDiscovery.h"

#include "StringUtils.h"

void WindowsComPortDiscovery::getAvailableComPorts(std::vector<std::string>& comPortVec) {
    static constexpr size_t WIN_REG_VAL_BUF_SIZE = 256;

    comPortVec.clear();
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        WCHAR valueName[WIN_REG_VAL_BUF_SIZE], comPort[WIN_REG_VAL_BUF_SIZE];
        DWORD valueNameSize, comPortSize, index = 0, type;
        while (true) {
            valueNameSize = sizeof(valueName) / sizeof(WCHAR);
            comPortSize = sizeof(comPort);
            if (RegEnumValue(hKey, index++, valueName, &valueNameSize, nullptr, &type, (LPBYTE) comPort, &comPortSize) != ERROR_SUCCESS) {
                break;
            }
            comPortVec.push_back(StringUtils::wcharToString(comPort));
        }
        RegCloseKey(hKey);
    }
}

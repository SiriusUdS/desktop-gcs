#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "UDPDeviceCtrlFlags.h"

struct DeviceInformation {
        uint8_t deviceID;
        uint32_t deviceTsMs;
        uint8_t deviceStatus;
        UDPDeviceCtrlFlags deviceCtrlFlags;
        std::vector<std::string> deviceLogs;
};

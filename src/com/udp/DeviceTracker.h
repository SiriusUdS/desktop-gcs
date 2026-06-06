#pragma once
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <optional>

#include "DeviceInformation.h"



class DeviceTracker {
public:
    std::optional<DeviceInformation> getDevice(uint8_t deviceId);
    bool deviceExists(uint8_t deviceId) const;
    bool updateDevice(uint8_t deviceId, DeviceInformation& deviceInformation);
    bool logInfo(uint8_t deviceId, const std::string& logMessage);
    std::vector<DeviceInformation> getAllDeviceInformation();
    std::vector<std::string> getDeviceLogs(uint8_t deviceId) const;
private:
    std::unordered_map<uint8_t, DeviceInformation> deviceMap;
    mutable std::shared_mutex trackerMutex;
};

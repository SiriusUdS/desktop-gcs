#include "DeviceTracker.h"

std::optional<DeviceInformation> DeviceTracker::getDevice(const uint8_t deviceId) {
    if (deviceExists(deviceId)) {
        return deviceMap[deviceId];
    }
    
    return std::nullopt;
}

bool DeviceTracker::deviceExists(const uint8_t deviceId) const {
    return deviceMap.contains(deviceId);
}

bool DeviceTracker::updateDevice(uint8_t deviceId, DeviceInformation& deviceInformation) {
    if (deviceExists(deviceId)) {
        std::vector<std::string> deviceLogs = deviceMap[deviceId].deviceLogs;
        for (auto log : deviceLogs) {
            deviceInformation.deviceLogs.push_back(log);
        }
    }
    deviceMap[deviceId] = deviceInformation;
    return true;
}

bool DeviceTracker::logInfo(uint8_t deviceId, const std::string& logMessage) {
    if (deviceExists(deviceId)) {
        deviceMap[deviceId].deviceLogs.push_back(logMessage);
        return true;
    }
    
    return false;
}
#include "DeviceTracker.h"

std::optional<DeviceInformation> DeviceTracker::getDevice(const uint8_t deviceId) {
    std::shared_lock lock(trackerMutex);
    if (deviceExists(deviceId)) {
        return deviceMap[deviceId];
    }
    
    return std::nullopt;
}

bool DeviceTracker::deviceExists(const uint8_t deviceId) const {
    return deviceMap.contains(deviceId);
}

bool DeviceTracker::updateDevice(uint8_t deviceId, DeviceInformation& deviceInformation) {
    std::unique_lock lock(trackerMutex);
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

std::vector<DeviceInformation> DeviceTracker::getAllDeviceInformation() {
    std::shared_lock lock(trackerMutex);
    
    std::vector<DeviceInformation> informations;
    informations.reserve(deviceMap.size());
    
    for (const auto& [id, data] : deviceMap) {
        informations.push_back(data);
    }
    
    return informations;
}

std::vector<std::string> DeviceTracker::getDeviceLogs(uint8_t deviceId) const {
    std::shared_lock lock(trackerMutex); 
    
    if (auto it = deviceMap.find(deviceId); it != deviceMap.end()) {
        return it->second.deviceLogs; 
    }

    return {};
}
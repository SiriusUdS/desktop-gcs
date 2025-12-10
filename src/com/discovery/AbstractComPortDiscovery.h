#pragma once

#include <string>
#include <vector>

/**
 * @class AbstractComPortDiscovery
 * @brief Abstract class for discovering available COM ports.
 */
class AbstractComPortDiscovery {
public:
    /**
     * @brief Gets the available COM ports.
     * @param comPortVec A vector to store the names of available COM ports.
     */
    virtual void getAvailableComPorts(std::vector<std::string>& comPortVec) = 0;
};

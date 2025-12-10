#pragma once

#include "AbstractComPortDiscovery.h"

/**
 * @class WindowsComPortDiscovery
 * @brief Discovers available COM ports on Windows.
 */
class WindowsComPortDiscovery : public AbstractComPortDiscovery {
public:
    void getAvailableComPorts(std::vector<std::string>& comPortVec) override;
};

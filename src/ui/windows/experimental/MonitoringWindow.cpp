#include "MonitoringWindow.h"

#include "ImGuiConfig.h"

const char* MonitoringWindow::name() const {
    return "Monitoring";
}

const char* MonitoringWindow::dockspace() const {
    return ImGuiConfig::Dockspace::MAP;
}

void MonitoringWindow::renderImpl() {
}

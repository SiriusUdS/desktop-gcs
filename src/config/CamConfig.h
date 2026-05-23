#pragma once

namespace CamConfig {
constexpr std::array<std::string_view, 3> ips = {
    "192.168.0.1", 
    "192.168.0.2", 
    "192.168.0.3"
};
constexpr int port = 554;
constexpr const char* user = "admin";
constexpr const char* password = "adminSirius";
constexpr const char* path = "/h264Preview_01_sub";
}
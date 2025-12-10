#pragma once

#include <future>

/**
 * @class TileSourceUrlConnTest
 * @brief Tests connection to the url tile source
 */
class TileSourceUrlConnTest {
public:
    bool successful();
    void startConnectivityTest(std::string url);

private:
    void performConnectivityTest(std::string url);

    std::mutex mtx;
    std::atomic_bool lastFetchSuccessful{false};
};

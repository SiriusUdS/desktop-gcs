#include "RecentDataSelector.h"

#include <doctest.h>

TEST_CASE("RecentDataSelector should correctly select data") {
    SUBCASE("Window size larger than 1") {
        std::vector<float> timeline = {0.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f};
        RecentDataSelector selector(2500);
        RecentDataSelector::Window window = selector.getWindow(timeline);
        CHECK(window.start == 3);
        CHECK(window.size == 3);
    }

    SUBCASE("Window size equal to 1") {
        std::vector<float> timeline = {0.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 8000.f, 10000.f};
        RecentDataSelector selector(500);
        RecentDataSelector::Window window = selector.getWindow(timeline);
        CHECK(window.start == 7);
        CHECK(window.size == 1);
    }

    SUBCASE("Window size same as input size") {
        std::vector<float> timeline = {0.0f, 1000.0f, 2000.0f, 3000.0f, 4000.0f, 5000.0f, 8000.f, 10000.f};
        RecentDataSelector selector(15000);
        RecentDataSelector::Window window = selector.getWindow(timeline);
        CHECK(window.start == 0);
        CHECK(window.size == 8);
    }
}

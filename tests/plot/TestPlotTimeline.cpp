#include "PlotTimeline.h"

#include <doctest.h>

TEST_CASE("PlotTimeline should add data correctly") {
    PlotTimeline data;
    data.add(1);
    data.add(4);
    data.add(3);

    CHECK(data.size() == 3);
    CHECK(data.at(0) == 1);
    CHECK(data.at(1) == 4);
    CHECK(data.at(2) == 3);
    CHECK(data.last() == 3);
}

TEST_CASE("PlotTimeline should clear values correctly") {
    PlotTimeline data;
    data.add(1);
    data.add(4);
    data.add(3);
    data.clear();

    CHECK(data.size() == 0);
}

TEST_CASE("PlotTimeline should erase old values correctly") {
    PlotTimeline data;
    for (size_t i = 0; i < 10; i++) {
        data.add((float) i + 1);
    }
    data.eraseOld(5);

    CHECK(data.at(0) == 6);
    CHECK(data.at(1) == 7);
    CHECK(data.last() == 10);
}

TEST_CASE("PlotTimeline should detect when data is unordered") {
    PlotTimeline data;
    for (size_t i = 0; i < 10; i++) {
        data.add((float) i + 1);
    }
    CHECK_FALSE(data.isUnordered());

    data.add(0);
    CHECK(data.isUnordered());

    data.add(1);
    CHECK(data.isUnordered());

    data.clear();
    CHECK_FALSE(data.isUnordered());

    data.add(1);
    data.add(2);
    CHECK_FALSE(data.isUnordered());
}

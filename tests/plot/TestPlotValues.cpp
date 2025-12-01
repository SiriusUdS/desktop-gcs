#include "PlotValues.h"

#include <doctest.h>

TEST_CASE("PlotValues should add data correctly") {
    PlotValues data;
    data.add(1);
    data.add(4);
    data.add(3);

    CHECK(data.size() == 3);
    CHECK(data.at(0) == 1);
    CHECK(data.at(1) == 4);
    CHECK(data.at(2) == 3);
    CHECK(data.last() == 3);
}

TEST_CASE("PlotValues should clear values correctly") {
    PlotValues data;
    data.add(1);
    data.add(4);
    data.add(3);
    data.clear();

    CHECK(data.size() == 0);
}

TEST_CASE("PlotValues should erase old values correctly") {
    PlotValues data;
    for (size_t i = 0; i < 10; i++) {
        data.add((float) i + 1);
    }
    data.eraseOld(5);

    CHECK(data.at(0) == 6);
    CHECK(data.at(1) == 7);
    CHECK(data.last() == 10);
}

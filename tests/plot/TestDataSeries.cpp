#include "DataSeries.h"

#include <doctest.h>

TEST_CASE("DataSeries should add data correctly") {
    DataSeries data(100);
    data.add(1);
    data.add(4);
    data.add(3);

    CHECK(data.raw().size() == 3);
    CHECK(data.raw().at(0) == 1);
    CHECK(data.raw().at(1) == 4);
    CHECK(data.raw().at(2) == 3);
    CHECK(data.raw().back() == 3);
    CHECK(data.compressed().size() == 3);
    CHECK(data.compressed().at(0) == 1);
    CHECK(data.compressed().at(1) == 4);
    CHECK(data.compressed().at(2) == 3);
    CHECK(data.compressed().back() == 3);
}

TEST_CASE("DataSeries should clear values correctly") {
    DataSeries data(100);
    data.add(1);
    data.add(4);
    data.add(3);
    data.clear();

    CHECK(data.raw().size() == 0);
    CHECK(data.compressed().size() == 0);
}

TEST_CASE("DataSeries should erase old values correctly") {
    DataSeries data(100);
    for (size_t i = 0; i < 10; i++) {
        data.add((float) i + 1);
    }
    data.eraseOld(5);

    CHECK(data.raw().at(0) == 6);
    CHECK(data.raw().at(1) == 7);
    CHECK(data.raw().back() == 10);
    CHECK(data.compressed().at(0) == 6);
    CHECK(data.compressed().at(1) == 7);
    CHECK(data.compressed().back() == 10);
}

TEST_CASE("DataSeries should compress correctly") {
    DataSeries data(3);
    for (size_t i = 0; i < 15; i++) {
        data.add((float) i);
    }
    data.compress();

    CHECK(data.raw().size() == 15);
    CHECK(data.compressed().size() == 3);
}

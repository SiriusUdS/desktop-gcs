#include "PlotDataCompression.h"
#include "PlotRawData.h"

#include <doctest.h>

const size_t TEST_PLOT_RAW_DATA_SIZE = 10;

TEST_CASE("PlotDataCompression mean compression") {
    std::vector<float> data;
    std::vector<float> compressed;

    for (float i = 0; i < TEST_PLOT_RAW_DATA_SIZE; i++) {
        data.push_back(i * 5);
    }

    SUBCASE("Should compress to correct size") {
        PlotDataCompression::meanCompression(data, compressed, 5);
        CHECK(compressed.size() == 5);

        PlotDataCompression::meanCompression(data, compressed, 8);
        CHECK(compressed.size() == 8);

        PlotDataCompression::meanCompression(data, compressed, 3);
        CHECK(compressed.size() == 3);

        PlotDataCompression::meanCompression(data, compressed, 1);
        CHECK(compressed.size() == 1);
    }

    SUBCASE("Shouldn't compress when target compression size higher or equal to current data size") {
        PlotDataCompression::meanCompression(data, compressed, TEST_PLOT_RAW_DATA_SIZE);
        CHECK(compressed.size() == TEST_PLOT_RAW_DATA_SIZE);

        PlotDataCompression::meanCompression(data, compressed, TEST_PLOT_RAW_DATA_SIZE + 1);
        CHECK(compressed.size() == TEST_PLOT_RAW_DATA_SIZE);

        PlotDataCompression::meanCompression(data, compressed, TEST_PLOT_RAW_DATA_SIZE + 10);
        CHECK(compressed.size() == TEST_PLOT_RAW_DATA_SIZE);

        PlotDataCompression::meanCompression(data, compressed, TEST_PLOT_RAW_DATA_SIZE + 9999);
        CHECK(compressed.size() == TEST_PLOT_RAW_DATA_SIZE);
    }

    SUBCASE("Should have correct values") {
        PlotDataCompression::meanCompression(data, compressed, 2);
        CHECK(compressed.at(0) == 10);
        CHECK(compressed.at(1) == 35);
    }
}

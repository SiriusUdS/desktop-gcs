#define DOCTEST_CONFIG_IMPLEMENT

#include "Logging.h"

#include <doctest.h>

int main(int argc, char** argv) {
    Logging::init();
    GCS_APP_LOG_INFO("Starting tests...");
    return doctest::Context(argc, argv).run();
}

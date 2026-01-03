#define DOCTEST_CONFIG_IMPLEMENT

#include "Logging.h"

#include <doctest.h>

int main(int argc, char** argv) {
    Logging::init();
    return doctest::Context(argc, argv).run();
}

#define DOCTEST_CONFIG_IMPLEMENT

#include "Application.h"
#include "Logging.h"
#include "LoggingWindow.h"

#include <doctest.h>

int main(int argc, char** argv) {
    Logging::init();
    Application::loggingWindow = std::make_shared<LoggingWindow>();
    return doctest::Context(argc, argv).run();
}

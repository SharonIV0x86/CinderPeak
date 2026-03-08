#include "PeakLogger.hpp"

int main() {
    // Settings toggle
    Logger::enableConsoleLogging = true;
    Logger::enableFileLogging = true;
    Logger::logFileName = "custom_logs.txt";

    // Logging calls
    LOG_INFO("System initialized");
    LOG_WARNING("This might be risky...");
    LOG_ERROR("Something failed badly!");
    LOG_DEBUG("This is a debug message");
    LOG_TRACE("This is a trace");
    LOG_CRITICAL("Hardware read write failure"); // Fixed typo here

    Logger::shutdown();

    return 0;
}
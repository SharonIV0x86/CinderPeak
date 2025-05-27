#include "PeakLogger.hpp"

int main() {
    // Toggle based on user preferences / build flags / env
    Logger::enableConsoleLogging = true;
    Logger::enableFileLogging = true;
    // Logger::logFileName = "custom_logs.txt";

    LOG_INFO("System initialized");
    LOG_WARNING("This might be risky...");
    LOG_ERROR("Something failed badly!");
    LOG_DEBUG("This is a debug message");
    LOG_TRACE("This is a trace");
    LOG_CRITICAL("Hardward read write faliure");

    // Logger::shutdown(); // optional cleanup
}

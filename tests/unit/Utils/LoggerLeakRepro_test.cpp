#include "GraphRuntime.hpp"

#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <system_error>

namespace
{
    std::size_t countOpenDescriptorsForPath(const std::filesystem::path &targetPath)
    {
        std::error_code ec;
        const auto expected = std::filesystem::absolute(targetPath).lexically_normal();

        std::size_t count = 0;
        for (const auto &entry : std::filesystem::directory_iterator("/proc/self/fd", ec))
        {
            if (ec)
            {
                break;
            }

            const auto linkTarget = std::filesystem::read_symlink(entry.path(), ec);
            if (ec)
            {
                ec.clear();
                continue;
            }

            if (std::filesystem::absolute(linkTarget).lexically_normal() == expected)
            {
                ++count;
            }
        }

        return count;
    }
}

TEST(GraphRuntimeLoggerLeakRepro, DisableFileLoggingShouldCloseActiveHandle)
{
    std::error_code ec;
    const auto logPath =
        std::filesystem::temp_directory_path() / "cinderpeak_logger_repro.log";

    std::filesystem::remove(logPath, ec);

    CinderPeak::GraphRuntime runtime;
    runtime.setConsoleLogging(false);
    runtime.setFileLogging(logPath.string());

    runtime.log(LogLevel::INFO, "logger leak repro probe");

    ASSERT_EQ(countOpenDescriptorsForPath(logPath), 1u)
        << "logger did not open the file as expected";

    runtime.disableFileLogging();

    EXPECT_EQ(countOpenDescriptorsForPath(logPath), 0u)
        << "file descriptor is still open after disabling file logging";

    Logger::shutdown();
    std::filesystem::remove(logPath, ec);
}
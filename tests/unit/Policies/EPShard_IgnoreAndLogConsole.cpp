#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"

#include <chrono>
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <regex>
#include <set>
#include <sstream>
#include <thread>

using namespace CinderPeak;

static const std::string kTestLogPath = "test_logconsole_policy.log";

class IgnoreAndLogConsolePolicyTest : public ::testing::Test {
public:
  PolicyConfiguration ignoreAndLogC_cfg{PolicyConfiguration::Ignore,
                                        PolicyConfiguration::LogConsole};
  PolicyHandler policy;

  PeakStatus sc_notFound = PeakStatus::NotFound();
  PeakStatus sc_invalidArgument = PeakStatus::InvalidArgument();
  PeakStatus sc_vertexAlreadyExists = PeakStatus::VertexAlreadyExists();
  PeakStatus sc_internalError = PeakStatus::InternalError();
  PeakStatus sc_edgeNotFound = PeakStatus::EdgeNotFound();
  PeakStatus sc_vertexNotFound = PeakStatus::VertexNotFound();
  PeakStatus sc_unimplemented = PeakStatus::Unimplemented();
  PeakStatus sc_alreadyExists = PeakStatus::AlreadyExists();
  PeakStatus sc_edgeAlreadyExists = PeakStatus::EdgeAlreadyExists();

  IgnoreAndLogConsolePolicyTest() : policy(ignoreAndLogC_cfg) {}

  void writeAllLogLevels(const std::string &msg) {
    policy.log(LogLevel::TRACE, msg);
    policy.log(LogLevel::DEBUG, msg);
    policy.log(LogLevel::INFO, msg);
    policy.log(LogLevel::WARNING, msg);
    policy.log(LogLevel::ERROR, msg);
    policy.log(LogLevel::CRITICAL, msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(120));
  }

  void verifyConsoleOutput(const std::string &expectedMessage,
                           const std::string &capturedOutput) {
    EXPECT_FALSE(capturedOutput.empty())
        << "LogConsole policy should print to console";

    EXPECT_TRUE(capturedOutput.find(expectedMessage) != std::string::npos)
        << "Console output should contain: " << expectedMessage
        << "\nActual output:\n"
        << capturedOutput;

    EXPECT_TRUE(capturedOutput.find("TRACE") != std::string::npos)
        << "Missing TRACE level in console output";
    EXPECT_TRUE(capturedOutput.find("DEBUG") != std::string::npos)
        << "Missing DEBUG level in console output";
    EXPECT_TRUE(capturedOutput.find("INFO") != std::string::npos)
        << "Missing INFO level in console output";
    EXPECT_TRUE(capturedOutput.find("WARN") != std::string::npos ||
                capturedOutput.find("WARNING") != std::string::npos)
        << "Missing WARNING level in console output";
    EXPECT_TRUE(capturedOutput.find("ERROR") != std::string::npos)
        << "Missing ERROR level in console output";
    EXPECT_TRUE(capturedOutput.find("CRITICAL") != std::string::npos)
        << "Missing CRITICAL level in console output";
  }
};

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_NotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_notFound));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Not Found", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_InvalidArgument) {
  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Invalid Argument");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Invalid Argument", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_VertexAlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_vertexAlreadyExists));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Vertex Already Exists", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_InternalError) {
  EXPECT_NO_THROW(policy.handleException(sc_internalError));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Internal Error");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Internal Error", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_EdgeNotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Edge Not Found", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_VertexNotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Vertex Not Found", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_Unimplemented) {
  EXPECT_NO_THROW(policy.handleException(sc_unimplemented));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Method is not implemented");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Method is not implemented", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_AlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_alreadyExists));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Already Exists", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_EdgeAlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_edgeAlreadyExists));

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Edge Already Exists", consoleOutput);
}

TEST_F(IgnoreAndLogConsolePolicyTest, IgnoreAndLogConsole_NoFileOutput) {
  writeAllLogLevels("Test Message");

  std::ifstream logFile(kTestLogPath);
  EXPECT_FALSE(logFile.good())
      << "LogConsole policy should not create a log file";
}
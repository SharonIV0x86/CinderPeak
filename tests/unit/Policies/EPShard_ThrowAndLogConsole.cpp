#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;

static const std::string kTestLogPath = "test_logconsole_policy.log";

class ThrowAndLogConsolePolicyTest : public ::testing::Test {
public:
  PolicyConfiguration throwAndLogC_cfg{PolicyConfiguration::Throw,
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

  ThrowAndLogConsolePolicyTest() : policy(throwAndLogC_cfg) {}

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

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_NotFound) {
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Not Found", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_InvalidArgument) {
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Invalid Argument");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Invalid Argument", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_VertexAlreadyExists) {
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_STREQ(vaex.what(), "Vertex already exists: Vertex Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Vertex Already Exists", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_InternalError) {
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_STREQ(ieex.what(), "Internal error: Internal Error");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Internal Error");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Internal Error", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_EdgeNotFound) {
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Edge Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Edge Not Found", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_VertexNotFound) {
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_STREQ(vnfex.what(), "Vertex not found: Vertex Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Vertex Not Found", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_Unimplemented) {
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_STREQ(unex.what(), "Unimplemented feature: Method is not "
                              "implemented, there has been an error.");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Method is not implemented");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Method is not implemented", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_AlreadyExists) {
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Already Exists: Resource Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Already Exists", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_EdgeAlreadyExists) {
  try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Edge already exists: Edge Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyConsoleOutput("Edge Already Exists", consoleOutput);
}

TEST_F(ThrowAndLogConsolePolicyTest, ThrowAndLogConsole_NoFileOutput) {
  writeAllLogLevels("Test Message");

  std::ifstream logFile(kTestLogPath);
  EXPECT_FALSE(logFile.good())
      << "LogConsole policy should not create a log file";
}
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

static const std::string kTestLogPath = "test_logfile_and_console_policy.log";

class ThrowAndLogFileAndConsolePolicyTest : public ::testing::Test {
public:
  PolicyConfiguration throwAndLogFC_cfg{PolicyConfiguration::Throw,
                                        PolicyConfiguration::ConsoleAndFile,
                                        kTestLogPath};
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

  ThrowAndLogFileAndConsolePolicyTest() : policy(throwAndLogFC_cfg) {}

  void SetUp() override {
    std::ofstream ofs(kTestLogPath, std::ios::trunc);
    ofs.close();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  void writeAllLogLevels(const std::string &msg) {
    policy.log(LogLevel::TRACE, msg);
    policy.log(LogLevel::DEBUG, msg);
    policy.log(LogLevel::INFO, msg);
    policy.log(LogLevel::WARNING, msg);
    policy.log(LogLevel::ERROR, msg);
    policy.log(LogLevel::CRITICAL, msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(120));
  }

  std::string readLogContent() const {
    std::ifstream in(kTestLogPath);
    if (!in.good())
      return "";
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
  }

  void verifyLogFormat(const std::string &expectedMessage) {
    std::string content = readLogContent();
    ASSERT_FALSE(content.empty())
        << "Log file is empty or not present at: " << kTestLogPath;

    std::regex linePattern(
        R"(\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\] \[(TRACE|DEBUG|INFO|WARN|WARNING|ERROR|CRITICAL)\] .+)");

    std::istringstream lines(content);
    std::string line;
    std::set<std::string> foundLevels;

    while (std::getline(lines, line)) {
      if (line.find(expectedMessage) == std::string::npos)
        continue;

      EXPECT_TRUE(std::regex_match(line, linePattern))
          << "Invalid log format: " << line;

      std::smatch m;
      if (std::regex_search(
              line, m,
              std::regex(
                  R"(\[(TRACE|DEBUG|INFO|WARN|WARNING|ERROR|CRITICAL)\])"))) {
        std::string lvl = m[1].str();
        if (lvl == "WARNING")
          lvl = "WARN";
        foundLevels.insert(lvl);
      }
    }

    const std::vector<std::string> expectedLevels = {
        "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};
    for (auto &lvl : expectedLevels) {
      EXPECT_TRUE(foundLevels.count(lvl))
          << "Missing log entry for level: " << lvl
          << " with message: " << expectedMessage << "\nFull log content:\n"
          << content;
    }
  }

  void verifyConsoleOutput(const std::string &expectedMessage,
                           const std::string &capturedOutput) {
    EXPECT_FALSE(capturedOutput.empty())
        << "ConsoleAndFile policy should print to console";

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

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_NotFound) {
  try {
    policy.handleException(sc_notFound);
    FAIL() << "Expected NotFoundException not thrown";
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Not Found");
  verifyConsoleOutput("Not Found", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_InvalidArgument) {
  try {
    policy.handleException(sc_invalidArgument);
    FAIL() << "Expected InvalidArgumentException not thrown";
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Invalid Argument");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Invalid Argument");
  verifyConsoleOutput("Invalid Argument", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_VertexAlreadyExists) {
  try {
    policy.handleException(sc_vertexAlreadyExists);
    FAIL() << "Expected VertexAlreadyExistsException not thrown";
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_STREQ(vaex.what(), "Vertex already exists: Vertex Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Vertex Already Exists");
  verifyConsoleOutput("Vertex Already Exists", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_InternalError) {
  try {
    policy.handleException(sc_internalError);
    FAIL() << "Expected InternalErrorException not thrown";
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_STREQ(ieex.what(), "Internal error: Internal Error");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Internal Error");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Internal Error");
  verifyConsoleOutput("Internal Error", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_EdgeNotFound) {
  try {
    policy.handleException(sc_edgeNotFound);
    FAIL() << "Expected EdgeNotFoundException not thrown";
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Edge Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Edge Not Found");
  verifyConsoleOutput("Edge Not Found", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_VertexNotFound) {
  try {
    policy.handleException(sc_vertexNotFound);
    FAIL() << "Expected VertexNotFoundException not thrown";
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_STREQ(vnfex.what(), "Vertex not found: Vertex Not Found");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Vertex Not Found");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Vertex Not Found");
  verifyConsoleOutput("Vertex Not Found", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_Unimplemented) {
  try {
    policy.handleException(sc_unimplemented);
    FAIL() << "Expected UnimplementedException not thrown";
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_STREQ(unex.what(), "Unimplemented feature: Method is not "
                              "implemented, there has been an error.");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Method is not implemented");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Method is not implemented");
  verifyConsoleOutput("Method is not implemented", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_AlreadyExists) {
  try {
    policy.handleException(sc_alreadyExists);
    FAIL() << "Expected AlreadyExistsException not thrown";
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Already Exists: Resource Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Already Exists");
  verifyConsoleOutput("Already Exists", consoleOutput);
}

TEST_F(ThrowAndLogFileAndConsolePolicyTest,
       ThrowAndLogFileAndConsole_EdgeAlreadyExists) {
  try {
    policy.handleException(sc_edgeAlreadyExists);
    FAIL() << "Expected EdgeAlreadyExistsException not thrown";
  } catch (const PeakExceptions::EdgeAlreadyExistsException &eaex) {
    EXPECT_STREQ(eaex.what(), "Edge already exists: Edge Already Exists");
  }

  testing::internal::CaptureStderr();
  writeAllLogLevels("Edge Already Exists");
  std::string consoleOutput = testing::internal::GetCapturedStderr();

  verifyLogFormat("Edge Already Exists");
  verifyConsoleOutput("Edge Already Exists", consoleOutput);
}

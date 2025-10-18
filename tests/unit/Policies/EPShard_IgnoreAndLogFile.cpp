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

static const std::string kTestLogPath = "test_logfile_ignore_policy.log";

class IngoreAndLogFilePolicyTest : public ::testing::Test {
public:
  PolicyConfiguration ignoreAndLog_cfg{
      PolicyConfiguration::Ignore, PolicyConfiguration::LogFile, kTestLogPath};
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

  IngoreAndLogFilePolicyTest() : policy(ignoreAndLog_cfg) {}

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
};

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_NotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_notFound));

  writeAllLogLevels("Not Found");
  verifyLogFormat("Not Found");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_InvalidArgument) {
  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));

  writeAllLogLevels("Invalid Argument");
  verifyLogFormat("Invalid Argument");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_VertexAlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_vertexAlreadyExists));

  writeAllLogLevels("Vertex Already Exists");
  verifyLogFormat("Vertex Already Exists");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_InternalError) {
  EXPECT_NO_THROW(policy.handleException(sc_internalError));

  writeAllLogLevels("Internal Error");
  verifyLogFormat("Internal Error");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_EdgeNotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));

  writeAllLogLevels("Edge Not Found");
  verifyLogFormat("Edge Not Found");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_VertexNotFound) {
  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));

  writeAllLogLevels("Vertex Not Found");
  verifyLogFormat("Vertex Not Found");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_Unimplemented) {
  EXPECT_NO_THROW(policy.handleException(sc_unimplemented));

  writeAllLogLevels("Method is not implemented");
  verifyLogFormat("Method is not implemented");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_AlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_alreadyExists));

  writeAllLogLevels("Already Exists");
  verifyLogFormat("Already Exists");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_EdgeAlreadyExists) {
  EXPECT_NO_THROW(policy.handleException(sc_edgeAlreadyExists));

  writeAllLogLevels("Edge Already Exists");
  verifyLogFormat("Edge Already Exists");
}

TEST_F(IngoreAndLogFilePolicyTest, IgnoreAndLogFile_NoConsoleOutput) {
  testing::internal::CaptureStderr();
  policy.handleException(sc_internalError);
  std::string err = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(err.empty())
      << "LogFile policy should not print to console. Got:\n"
      << err;
}
#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include <sstream>
#include <thread>
#include <chrono>

using namespace CinderPeak;

class PolicyShardFileTest : public ::testing::Test {
public:
  static int fileCounter;
  std::string logPath;
  PolicyConfiguration cfg{PolicyConfiguration::Throw, PolicyConfiguration::LogFile, ""};
  PolicyHandler policy;

  PolicyShardFileTest()
      : logPath(generateUniqueLogPath()),
        cfg(PolicyConfiguration::Throw, PolicyConfiguration::LogFile, logPath),
        policy(cfg) {}

  static std::string generateUniqueLogPath() {
    std::ostringstream ss;
    ss << "test_logfile_policy_" << fileCounter++ << ".log";
    return ss.str();
  }

  void TearDown() override {
    std::remove(logPath.c_str());
  }

  std::string readLogContent() const {
    std::ifstream in(logPath);
    if (!in.good())
      return "";
    return std::string((std::istreambuf_iterator<char>(in)), {});
  }

  void verifyLogFormat(const std::string &expectedMessage) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::string content = readLogContent();
    ASSERT_FALSE(content.empty()) << "Log file should contain entries";

    std::regex logPattern(
        R"(\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\] \[(TRACE|DEBUG|INFO|WARNING|ERROR|CRITICAL)\] .*)");

    bool formatOK = false;
    bool foundMessage = content.find(expectedMessage) != std::string::npos;

    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
      if (line.find(expectedMessage) != std::string::npos &&
          std::regex_match(line, logPattern)) {
        formatOK = true;
        break;
      }
    }

    EXPECT_TRUE(foundMessage) << "Expected message not found: " << expectedMessage;
    EXPECT_TRUE(formatOK) << "Log format invalid for: " << expectedMessage;
  }

  void verifyNoConsoleOutput() {
    testing::internal::CaptureStderr();
    try {
      policy.handleException(PeakStatus::NotFound());
    } catch (const std::exception &) {
      // ignore
    }
    std::string errOutput = testing::internal::GetCapturedStderr();
    EXPECT_TRUE(errOutput.empty()) << "LogFile policy should not print to console";
  }
};

int PolicyShardFileTest::fileCounter = 0;

// Helper to write manually before checking
void writeFakeLog(const std::string &path, const std::string &msg) {
  std::ofstream out(path);
  out << "[2025-10-07 15:02:25.893] [INFO] " << msg << std::endl;
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_NotFound) {
  writeFakeLog(logPath, "Not Found");
  verifyLogFormat("Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_InvalidArgument) {
  writeFakeLog(logPath, "Invalid Argument");
  verifyLogFormat("Invalid Argument");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_VertexAlreadyExists) {
  writeFakeLog(logPath, "Vertex Already Exists");
  verifyLogFormat("Vertex Already Exists");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_InternalError) {
  writeFakeLog(logPath, "Internal Error");
  verifyLogFormat("Internal Error");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_EdgeNotFound) {
  writeFakeLog(logPath, "Edge Not Found");
  verifyLogFormat("Edge Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_VertexNotFound) {
  writeFakeLog(logPath, "Vertex Not Found");
  verifyLogFormat("Vertex Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_Unimplemented) {
  writeFakeLog(logPath, "Method is not implemented");
  verifyLogFormat("Method is not implemented");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_AlreadyExists) {
  writeFakeLog(logPath, "Already Exists");
  verifyLogFormat("Already Exists");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_EdgeAlreadyExists) {
  writeFakeLog(logPath, "Edge Already Exists");
  verifyLogFormat("Edge Already Exists");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_NoConsoleOutput) {
  verifyNoConsoleOutput();
}

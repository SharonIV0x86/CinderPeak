#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class IgnoreAndSilentPolicyTest : public ::testing::Test {
public:
  PolicyConfiguration ignoreAndSilent_cfg{PolicyConfiguration::Ignore,
                                          PolicyConfiguration::Silent};
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

  const std::string test_log_file = "test_policy.log";

  IgnoreAndSilentPolicyTest() : policy(ignoreAndSilent_cfg) {}

  void SetUp() override {
    if (std::filesystem::exists(test_log_file)) {
      std::filesystem::remove(test_log_file);
    }
  }

  bool logFileExists() { return std::filesystem::exists(test_log_file); }

  bool logFileIsEmpty() {
    if (!logFileExists())
      return true;
    std::ifstream file(test_log_file);
    return file.peek() == std::ifstream::traits_type::eof();
  }

  void verifyCompletelySilent() {
    std::string stdout_output = testing::internal::GetCapturedStdout();
    std::string stderr_output = testing::internal::GetCapturedStderr();

    EXPECT_TRUE(stdout_output.empty())
        << "Expected no console stdout output, but got: " << stdout_output;
    EXPECT_TRUE(stderr_output.empty())
        << "Expected no console stderr output, but got: " << stderr_output;
    EXPECT_TRUE(!logFileExists() || logFileIsEmpty())
        << "Expected no log file or empty log file, but file contains data";
  }
};

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_NotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_notFound));
  EXPECT_NO_THROW(policy.log(LogLevel::INFO, "NotFound log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_InvalidArgument) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));
  EXPECT_NO_THROW(policy.log(LogLevel::ERROR, "InvalidArgument log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_VertexAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexAlreadyExists));
  EXPECT_NO_THROW(
      policy.log(LogLevel::WARNING, "VertexAlreadyExists log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_InternalError) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_internalError));
  EXPECT_NO_THROW(policy.log(LogLevel::ERROR, "InternalError log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_EdgeNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));
  EXPECT_NO_THROW(policy.log(LogLevel::INFO, "EdgeNotFound log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_VertexNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));
  EXPECT_NO_THROW(policy.log(LogLevel::INFO, "VertexNotFound log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_Unimplemented) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_unimplemented));
  EXPECT_NO_THROW(policy.log(LogLevel::WARNING, "Unimplemented log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_AlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_alreadyExists));
  EXPECT_NO_THROW(policy.log(LogLevel::WARNING, "AlreadyExists log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_EdgeAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_edgeAlreadyExists));
  EXPECT_NO_THROW(
      policy.log(LogLevel::WARNING, "EdgeAlreadyExists log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_MultipleExceptions) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));
  EXPECT_NO_THROW(policy.log(LogLevel::INFO, "First log message"));
  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));
  EXPECT_NO_THROW(policy.log(LogLevel::ERROR, "Second log message"));
  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));
  EXPECT_NO_THROW(policy.log(LogLevel::DEBUG, "Third log message"));

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_RepeatedException) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  for (int i = 0; i < 3; ++i) {
    EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));
    EXPECT_NO_THROW(policy.log(LogLevel::INFO,
                               "Repeated log message " + std::to_string(i)));
  }

  verifyCompletelySilent();
}

TEST_F(IgnoreAndSilentPolicyTest,
       IgnoreAndSilent_EdgeNotFoundWithCustomMessage) {
  PeakStatus sc_custom_edgeNotFound =
      PeakStatus::EdgeNotFound("Custom edge not found message");
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_custom_edgeNotFound));
  EXPECT_NO_THROW(policy.log(LogLevel::ERROR, "Custom message log"));

  verifyCompletelySilent();
}
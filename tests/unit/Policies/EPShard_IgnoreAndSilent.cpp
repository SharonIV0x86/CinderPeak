#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
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

  IgnoreAndSilentPolicyTest() : policy(ignoreAndSilent_cfg) {}
};

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_NotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_notFound));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_InvalidArgument) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_VertexAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexAlreadyExists));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_InternalError) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_internalError));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_EdgeNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_VertexNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_Unimplemented) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_unimplemented));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_AlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_alreadyExists));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_EdgeAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_edgeAlreadyExists));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_MultipleExceptions) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_vertexNotFound));
  EXPECT_NO_THROW(policy.handleException(sc_edgeNotFound));
  EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest, IgnoreAndSilent_RepeatedException) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  for (int i = 0; i < 3; ++i) {
    EXPECT_NO_THROW(policy.handleException(sc_invalidArgument));
  }

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}

TEST_F(IgnoreAndSilentPolicyTest,
       IgnoreAndSilent_EdgeNotFoundWithCustomMessage) {
  PeakStatus sc_custom_edgeNotFound =
      PeakStatus::EdgeNotFound("Custom edge not found message");
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();

  EXPECT_NO_THROW(policy.handleException(sc_custom_edgeNotFound));

  std::string stdout_output = testing::internal::GetCapturedStdout();
  std::string stderr_output = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(stdout_output.empty())
      << "Expected no console stdout output, but got: " << stdout_output;
  EXPECT_TRUE(stderr_output.empty())
      << "Expected no console stderr output, but got: " << stderr_output;
}
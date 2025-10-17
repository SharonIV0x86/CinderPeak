#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

// Fixture for shared policy setup
class ThrowAndSilentPolicyTest : public ::testing::Test {
public:
  PolicyConfiguration throwAndSilent_cfg{PolicyConfiguration::Throw,
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

  ThrowAndSilentPolicyTest() : policy(throwAndSilent_cfg) {}

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

// 1. NotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_NotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }
  policy.log(LogLevel::INFO, "NotFound log message");
  verifyCompletelySilent();
}

// 2. InvalidArgument exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_InvalidArgument) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
  }
  policy.log(LogLevel::ERROR, "InvalidArgument log message");
  verifyCompletelySilent();
}

// 3. VertexAlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_VertexAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_STREQ(vaex.what(), "Vertex already exists: Vertex Already Exists");
  }
  policy.log(LogLevel::WARNING, "VertexAlreadyExists log message");
  verifyCompletelySilent();
}

// 4. InternalError exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_InternalError) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_STREQ(ieex.what(), "Internal error: Internal Error");
  }
  policy.log(LogLevel::ERROR, "InternalError log message");
  verifyCompletelySilent();
}

// 5. EdgeNotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Edge Not Found");
  }
  policy.log(LogLevel::INFO, "EdgeNotFound log message");
  verifyCompletelySilent();
}

// 6. VertexNotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_VertexNotFound) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_STREQ(vnfex.what(), "Vertex not found: Vertex Not Found");
  }
  policy.log(LogLevel::INFO, "VertexNotFound log message");
  verifyCompletelySilent();
}

// 7. Unimplemented exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_Unimplemented) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_STREQ(unex.what(), "Unimplemented feature: Method is not "
                              "implemented, there has been an error.");
  }
  policy.log(LogLevel::WARNING, "Unimplemented log message");
  verifyCompletelySilent();
}

// 8. AlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_AlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Already Exists: Resource Already Exists");
  }
  policy.log(LogLevel::WARNING, "AlreadyExists log message");
  verifyCompletelySilent();
}

// 9. EdgeAlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeAlreadyExists) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &eaex) {
    EXPECT_STREQ(eaex.what(), "Edge already exists: Edge Already Exists");
  }
  policy.log(LogLevel::WARNING, "EdgeAlreadyExists log message");
  verifyCompletelySilent();
}

// 10. Multiple exceptions in sequence
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_MultipleExceptions) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &) {
  }
  policy.log(LogLevel::INFO, "First log message");
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &) {
  }
  policy.log(LogLevel::INFO, "Second log message");
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &) {
  }
  policy.log(LogLevel::INFO, "Third log message");
  verifyCompletelySilent();
}

// 11. Repeated exception handling
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_RepeatedException) {
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  for (int i = 0; i < 3; ++i) {
    try {
      policy.handleException(sc_invalidArgument);
    } catch (const PeakExceptions::InvalidArgumentException &iaex) {
      EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
    }
    policy.log(LogLevel::INFO, "Repeated log message " + std::to_string(i));
  }
  verifyCompletelySilent();
}

// 12. EdgeNotFound with custom message
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeNotFoundWithCustomMessage) {
  PeakStatus sc_custom_edgeNotFound =
      PeakStatus::EdgeNotFound("Custom edge not found message");
  testing::internal::CaptureStdout();
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_custom_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Custom edge not found message");
  }
  policy.log(LogLevel::ERROR, "Custom message log");
  verifyCompletelySilent();
}
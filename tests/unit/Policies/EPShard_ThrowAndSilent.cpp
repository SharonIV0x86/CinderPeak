#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
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

  ThrowAndSilentPolicyTest() : policy(throwAndSilent_cfg) {}
};

// 1. NotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_NotFound) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 2. InvalidArgument exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_InvalidArgument) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 3. VertexAlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_VertexAlreadyExists) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_STREQ(vaex.what(), "Vertex already exists: Vertex Already Exists");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 4. InternalError exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_InternalError) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_STREQ(ieex.what(), "Internal error: Internal Error");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 5. EdgeNotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeNotFound) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Edge Not Found");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 6. VertexNotFound exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_VertexNotFound) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_STREQ(vnfex.what(), "Vertex not found: Vertex Not Found");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 7. Unimplemented exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_Unimplemented) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_STREQ(unex.what(), "Unimplemented feature: Method is not "
                              "implemented, there has been an error.");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 8. AlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_AlreadyExists) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Already Exists: Resource Already Exists");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 9. EdgeAlreadyExists exception
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeAlreadyExists) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &eaex) {
    EXPECT_STREQ(eaex.what(), "Edge already exists: Edge Already Exists");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 10. Multiple exceptions in sequence
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_MultipleExceptions) {
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &) {
  }
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &) {
  }
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &) {
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 11. Repeated exception handling
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_RepeatedException) {
  testing::internal::CaptureStdout();
  for (int i = 0; i < 3; ++i) {
    try {
      policy.handleException(sc_invalidArgument);
    } catch (const PeakExceptions::InvalidArgumentException &iaex) {
      EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
    }
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}

// 12. EdgeNotFound with custom message
TEST_F(ThrowAndSilentPolicyTest, ThrowAndSilent_EdgeNotFoundWithCustomMessage) {
  PeakStatus sc_custom_edgeNotFound =
      PeakStatus::EdgeNotFound("Custom edge not found message");
  testing::internal::CaptureStdout();
  try {
    policy.handleException(sc_custom_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Custom edge not found message");
  }
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(output.empty())
      << "Expected no console output, but got: " << output;
}
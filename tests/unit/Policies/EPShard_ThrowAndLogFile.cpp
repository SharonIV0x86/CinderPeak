#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <fstream>

using namespace CinderPeak;

class PolicyShardFileTest : public ::testing::Test {
public:
  const std::string logPath = "test_logfile_policy.log";
  PolicyConfiguration cfg{PolicyConfiguration::Throw, PolicyConfiguration::LogFile, logPath};
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

  PolicyShardFileTest() : policy(cfg) {}

  void TearDown() override { std::remove(logPath.c_str()); }

  std::string readLogContent() const {
    std::ifstream in(logPath);
    if (!in.good())
      return "";
    return std::string((std::istreambuf_iterator<char>(in)), {});
  }

  void verifyLogContains(const std::string &expected) {
    std::string content = readLogContent();
    ASSERT_FALSE(content.empty()) << "Log file should contain entries";
    EXPECT_NE(content.find(expected), std::string::npos)
        << "Expected log content missing: " << expected;
  }

  void verifyNoConsoleOutput() {
  testing::internal::CaptureStderr();
  try {
    policy.handleException(sc_notFound);
  } catch (const std::exception &) {
   
  }
  std::string errOutput = testing::internal::GetCapturedStderr();
  EXPECT_TRUE(errOutput.empty()) << "LogFile policy should not print to console";
}

};



TEST_F(PolicyShardFileTest, ThrowAndLogFile_NotFound) {
  std::cerr << "LOG PATH: " << logPath << std::endl;

  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &ex) {
    EXPECT_STREQ(ex.what(), "Resource Not Found: Not Found");
  }
  verifyLogContains("Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_InvalidArgument) {
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &ex) {
    EXPECT_STREQ(ex.what(), "Invalid argument: Invalid Argument");
  }
  verifyLogContains("Invalid Argument");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_VertexAlreadyExists) {
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &ex) {
    EXPECT_STREQ(ex.what(), "Vertex already exists: Vertex Already Exists");
  }
  verifyLogContains("Vertex Already Exists");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_InternalError) {
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ex) {
    EXPECT_STREQ(ex.what(), "Internal error: Internal Error");
  }
  verifyLogContains("Internal Error");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_EdgeNotFound) {
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &ex) {
    EXPECT_STREQ(ex.what(), "Edge not found: Edge Not Found");
  }
  verifyLogContains("Edge Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_VertexNotFound) {
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &ex) {
    EXPECT_STREQ(ex.what(), "Vertex not found: Vertex Not Found");
  }
  verifyLogContains("Vertex Not Found");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_Unimplemented) {
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &ex) {
   EXPECT_STREQ(ex.what(), "Unimplemented feature: Method is not implemented, there has been an error.");

  }
  verifyLogContains("Method is not implemented");

}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_AlreadyExists) {
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &ex) {
  EXPECT_STREQ(ex.what(), "Already Exists: Resource Already Exists");

  }
  verifyLogContains("Already Exists");
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_EdgeAlreadyExists) {
  try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &ex) {
    EXPECT_STREQ(ex.what(), "Edge already exists: Edge Already Exists");
  }
  verifyLogContains("Edge Already Exists");
}


TEST_F(PolicyShardFileTest, ThrowAndLogFile_NoConsoleOutput) {
  verifyNoConsoleOutput();
}

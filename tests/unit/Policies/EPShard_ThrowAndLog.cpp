#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>

using namespace CinderPeak;

class PolicyShardTest : public ::testing::Test {
public:
  PolicyConfiguration throwAndLog_cfg{PolicyConfiguration::Throw,
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

  PolicyShardTest() : policy(throwAndLog_cfg) {}
};

#define EXPECT_WHAT_CONTAINS(EXCEPTION, SUBSTRING) \
  EXPECT_NE(std::string(EXCEPTION.what()).find(SUBSTRING), std::string::npos)

TEST_F(PolicyShardTest, ThrowAndLogConsole_NotFound) {
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_WHAT_CONTAINS(nfex, "Not Found");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_InvalidArgument) {
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_WHAT_CONTAINS(iaex, "Invalid Argument");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexAlreadyExists) {
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_WHAT_CONTAINS(vaex, "Vertex Already Exists");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_InternalError) {
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_WHAT_CONTAINS(ieex, "Internal Error");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeNotFound) {
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_WHAT_CONTAINS(enfex, "Edge Not Found");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexNotFound) {
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_WHAT_CONTAINS(vnfex, "Vertex Not Found");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_Unimplemented) {
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_WHAT_CONTAINS(unex, "Method is not implemented");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_AlreadyExists) {
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_WHAT_CONTAINS(aeex, "Resource Already Exists");
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeAlreadyExists) {
  try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &aeex) {
    EXPECT_WHAT_CONTAINS(aeex, "Edge Already Exists");
  }
}

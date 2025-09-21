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

TEST_F(PolicyShardTest, ThrowAndLogConsole_NotFound) {
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_InvalidArgument) {
  try {
    policy.handleException(sc_invalidArgument);
  } catch (const PeakExceptions::InvalidArgumentException &iaex) {
    EXPECT_STREQ(iaex.what(), "Invalid argument: Invalid Argument");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexAlreadyExists) {
  try {
    policy.handleException(sc_vertexAlreadyExists);
  } catch (const PeakExceptions::VertexAlreadyExistsException &vaex) {
    EXPECT_STREQ(vaex.what(), "Vertex already exists: Vertex Already Exists");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_InternalError) {
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &ieex) {
    EXPECT_STREQ(ieex.what(), "Internal error: Internal Error");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeNotFound) {
  try {
    policy.handleException(sc_edgeNotFound);
  } catch (const PeakExceptions::EdgeNotFoundException &enfex) {
    EXPECT_STREQ(enfex.what(), "Edge not found: Edge Not Found");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexNotFound) {
  try {
    policy.handleException(sc_vertexNotFound);
  } catch (const PeakExceptions::VertexNotFoundException &vnfex) {
    EXPECT_STREQ(vnfex.what(), "Vertex not found: Vertex Not Found");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_Unimplemented) {
  try {
    policy.handleException(sc_unimplemented);
  } catch (const PeakExceptions::UnimplementedException &unex) {
    EXPECT_STREQ(unex.what(), "Unimplemented feature: Method is not "
                              "implemented, there has been an error.");
  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_AlreadyExists) {
  try {
    policy.handleException(sc_alreadyExists);
  } catch (const PeakExceptions::AlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Already Exists: Resource Already Exists");

  }
}
TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeAlreadyExists){
   try {
    policy.handleException(sc_edgeAlreadyExists);
  } catch (const PeakExceptions::EdgeAlreadyExistsException &aeex) {
    EXPECT_STREQ(aeex.what(), "Edge already exists: Edge Already Exists");
  }
}
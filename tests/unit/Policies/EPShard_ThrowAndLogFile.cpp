#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <fstream>
using namespace CinderPeak;

class PolicyShardFileTest : public ::testing::Test {
public:
  PolicyConfiguration throwAndLogFile_cfg{PolicyConfiguration::Throw,
                                          PolicyConfiguration::LogFile};
  PolicyHandler policy;

  PeakStatus sc_notFound = PeakStatus::NotFound();
  PeakStatus sc_internalError = PeakStatus::InternalError();

  PolicyShardFileTest() : policy(throwAndLogFile_cfg) {}

  void TearDown() override { std::remove("peak_logs.log"); }
};

TEST_F(PolicyShardFileTest, ThrowAndLogFile_NotFound) {
  try {
    policy.handleException(sc_notFound);
  } catch (const PeakExceptions::NotFoundException &nfex) {
    EXPECT_STREQ(nfex.what(), "Resource Not Found: Not Found");
  }
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_InternalError) {
  try {
    policy.handleException(sc_internalError);
  } catch (const PeakExceptions::InternalErrorException &iex) {
    EXPECT_STREQ(iex.what(), "Internal error: Internal Error");
  }
}

TEST_F(PolicyShardFileTest, ThrowAndLogFile_WritesToFile) {
  // Trigger a file log manually
  Logger::log(LogLevel::INFO, "Test log entry", LoggingPolicy::LogFile, "peak_logs.log");

  std::ifstream in("peak_logs.log");
  ASSERT_TRUE(in.good()) << "Log file should be created when using LogFile policy";

  std::string content((std::istreambuf_iterator<char>(in)), {});
  in.close();

  EXPECT_NE(content.find("Test log entry"), std::string::npos)
      << "Expected log message not found in peak_logs.log";
}

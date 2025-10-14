#include "CinderExceptions.hpp"
#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <string_view>
#include <iostream>

using namespace CinderPeak;

class ConsoleCapture {
public:
#if defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION
  ConsoleCapture() : active(true) {
    ::testing::internal::CaptureStdout();
    ::testing::internal::CaptureStderr();
  }
#else
  ConsoleCapture() : active(true),
                     old_cout_buf(nullptr), old_cerr_buf(nullptr) {
    old_cout_buf = std::cout.rdbuf(out_ss.rdbuf());
    old_cerr_buf = std::cerr.rdbuf(err_ss.rdbuf());
  }
#endif

  ~ConsoleCapture() {
    if (active) {
      try {
        (void)stopAndGet();
      } catch (...) {}
    }
  }

  std::string stopAndGet() {
    if (!active) return {};
#if defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION
    std::string out = ::testing::internal::GetCapturedStdout();
    std::string err = ::testing::internal::GetCapturedStderr();
    active = false;
    return out + err;
#else
    std::cout.rdbuf(old_cout_buf);
    std::cerr.rdbuf(old_cerr_buf);
    old_cout_buf = nullptr;
    old_cerr_buf = nullptr;
    active = false;
    return out_ss.str() + err_ss.str();
#endif
  }

private:
  bool active;

#if !(defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION)
  std::ostringstream out_ss;
  std::ostringstream err_ss;
  std::streambuf* old_cout_buf;
  std::streambuf* old_cerr_buf;
#endif
};

class ConsoleCapture {
public:
#if defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION
  ConsoleCapture() : active(true) {
    ::testing::internal::CaptureStdout();
    ::testing::internal::CaptureStderr();
  }
#else
  ConsoleCapture() : active(true),
                     old_cout_buf(nullptr), old_cerr_buf(nullptr) {
    old_cout_buf = std::cout.rdbuf(out_ss.rdbuf());
    old_cerr_buf = std::cerr.rdbuf(err_ss.rdbuf());
  }
#endif

  ~ConsoleCapture() {
    if (active) {
      try {
        (void)stopAndGet();
      } catch (...) {}
    }
  }

  std::string stopAndGet() {
    if (!active) return {};
#if defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION
    std::string out = ::testing::internal::GetCapturedStdout();
    std::string err = ::testing::internal::GetCapturedStderr();
    active = false;
    return out + err;
#else
    std::cout.rdbuf(old_cout_buf);
    std::cerr.rdbuf(old_cerr_buf);
    old_cout_buf = nullptr;
    old_cerr_buf = nullptr;
    active = false;
    return out_ss.str() + err_ss.str();
#endif
  }

private:
  bool active;

#if !(defined(GTEST_HAS_STREAM_REDIRECTION) && GTEST_HAS_STREAM_REDIRECTION)
  std::ostringstream out_ss;
  std::ostringstream err_ss;
  std::streambuf* old_cout_buf;
  std::streambuf* old_cerr_buf;
#endif
};

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

static constexpr std::string_view kNotFoundMsg = "Resource Not Found: Not Found";
static constexpr std::string_view kInvalidArgMsg = "Invalid argument: Invalid Argument";
static constexpr std::string_view kVertexAlreadyExistsMsg = "Vertex already exists: Vertex Already Exists";
static constexpr std::string_view kInternalErrorMsg = "Internal error: Internal Error";
static constexpr std::string_view kEdgeNotFoundMsg = "Edge not found: Edge Not Found";
static constexpr std::string_view kVertexNotFoundMsg = "Vertex not found: Vertex Not Found";
static constexpr std::string_view kUnimplementedMsg = "Unimplemented feature: Method is not implemented, there has been an error.";
static constexpr std::string_view kAlreadyExistsMsg = "Already Exists: Resource Already Exists";
static constexpr std::string_view kEdgeAlreadyExistsMsg = "Edge already exists: Edge Already Exists";

template <typename ExceptionT>
void assertPolicyThrowsAndLogs(PolicyHandler& policy,
                               const PeakStatus& status,
                               std::string_view expected_msg) {
  ConsoleCapture cap;
  try {
    policy.handleException(status);
    std::string combined = cap.stopAndGet();
    (void)combined;
    FAIL() << "Expected exception to be thrown";
  } catch (const ExceptionT& ex) {
    EXPECT_STREQ(ex.what(), std::string(expected_msg).c_str());
    std::string combined = cap.stopAndGet();
    EXPECT_NE(combined.find(ex.what()), std::string::npos)
        << "Console output did not contain exception message. Output:\n"
        << combined;
  } catch (...) {
    FAIL() << "Unexpected exception type thrown";
  }
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_NotFound) {
  assertPolicyThrowsAndLogs<PeakExceptions::NotFoundException>(policy, sc_notFound, kNotFoundMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_InvalidArgument) {
  assertPolicyThrowsAndLogs<PeakExceptions::InvalidArgumentException>(policy, sc_invalidArgument, kInvalidArgMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexAlreadyExists) {
  assertPolicyThrowsAndLogs<PeakExceptions::VertexAlreadyExistsException>(policy, sc_vertexAlreadyExists, kVertexAlreadyExistsMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_InternalError) {
  assertPolicyThrowsAndLogs<PeakExceptions::InternalErrorException>(policy, sc_internalError, kInternalErrorMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeNotFound) {
  assertPolicyThrowsAndLogs<PeakExceptions::EdgeNotFoundException>(policy, sc_edgeNotFound, kEdgeNotFoundMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_VertexNotFound) {
  assertPolicyThrowsAndLogs<PeakExceptions::VertexNotFoundException>(policy, sc_vertexNotFound, kVertexNotFoundMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_Unimplemented) {
  assertPolicyThrowsAndLogs<PeakExceptions::UnimplementedException>(policy, sc_unimplemented, kUnimplementedMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_AlreadyExists) {
  assertPolicyThrowsAndLogs<PeakExceptions::AlreadyExistsException>(policy, sc_alreadyExists, kAlreadyExistsMsg);
}

TEST_F(PolicyShardTest, ThrowAndLogConsole_EdgeAlreadyExists) {
  assertPolicyThrowsAndLogs<PeakExceptions::EdgeAlreadyExistsException>(policy, sc_edgeAlreadyExists, kEdgeAlreadyExistsMsg);
}

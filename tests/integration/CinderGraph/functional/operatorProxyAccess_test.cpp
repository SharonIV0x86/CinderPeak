#include "StorageEngine/DebugUtils.hpp"
#include <gtest/gtest.h>

using namespace CinderPeak;

struct CustomVertex : public CinderPeak::CinderVertex {
  explicit CustomVertex(int id) {
    __id_ = id;
  }

  bool operator==(const CustomVertex &other) const {
    return __id_ == other.__id_;
  }

  bool operator<(const CustomVertex &other) const {
    return __id_ < other.__id_;
  }
};

TEST(CinderGraphOperatorProxyTest, EdgeFormattingSupportsCustomVertexTypes) {

  CustomVertex v1{1};
  CustomVertex v2{2};

  auto formatted = edgeStr(v1, v2);

  EXPECT_NE(formatted.find("Edge("), std::string::npos);
  EXPECT_NE(formatted.find("id:1"), std::string::npos);
  EXPECT_NE(formatted.find("id:2"), std::string::npos);
}
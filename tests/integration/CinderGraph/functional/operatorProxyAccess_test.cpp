#include "StorageEngine/DebugUtils.hpp"
#include <gtest/gtest.h>

using namespace CinderPeak;

struct CustomVertex {
  int __id_;

  bool operator==(const CustomVertex &other) const {
    return __id_ == other.__id_;
  }

  bool operator<(const CustomVertex &other) const {
    return __id_ < other.__id_;
  }
};

namespace std {
template <> struct hash<CustomVertex> {
  size_t operator()(const CustomVertex &v) const {
    return hash<int>()(v.__id_);
  }
};
} // namespace std

TEST(CinderGraphOperatorProxyTest, EdgeFormattingSupportsCustomVertexTypes) {

  CustomVertex v1{1};
  CustomVertex v2{2};

  auto formatted = edgeStr(v1, v2);

  EXPECT_NE(formatted.find("Edge("), std::string::npos);
  EXPECT_NE(formatted.find("id:1"), std::string::npos);
  EXPECT_NE(formatted.find("id:2"), std::string::npos);
}
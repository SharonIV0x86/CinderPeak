#include "CinderPeak.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class ListVertex : public CinderVertex {
public:
  int data;
  ListVertex(int d = 0) : data(d) {}
  bool operator==(const ListVertex &other) const { return data == other.data; }
};

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  GraphCreationOptions directedOpts;
  GraphCreationOptions undirectedOpts;

  CinderGraphFunctionalTest()
      : directedOpts({GraphCreationOptions::Directed}),
        undirectedOpts({GraphCreationOptions::Undirected}) {}
};

TEST_F(CinderGraphFunctionalTest, AddVertexPrimitive) {
  CinderGraph<int, int> intGraph;

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_FALSE(intGraph.addVertex(1).second); // duplicate
}

TEST_F(CinderGraphFunctionalTest, AddVertexString) {
  CinderGraph<std::string, float> stringGraph;

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_FALSE(stringGraph.addVertex("A").second); // duplicate
}

TEST_F(CinderGraphFunctionalTest, AddVertexCustomType) {
  CinderGraph<ListVertex, float> customGraph;

  ListVertex v1(10);
  ListVertex v2(20);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_FALSE(customGraph.addVertex(v1).second); // duplicate
}
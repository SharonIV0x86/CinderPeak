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

TEST_F(CinderGraphFunctionalTest, RemoveVertexPrimitive) {
  CinderGraph<int, int> intGraph;

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_EQ(intGraph.numVertices(), 3);

  EXPECT_TRUE(intGraph.removeVertex(1));
  EXPECT_TRUE(intGraph.removeVertex(2));
  EXPECT_FALSE(intGraph.removeVertex(4));

  EXPECT_EQ(intGraph.numVertices(), 1);
}

TEST_F(CinderGraphFunctionalTest, RemoveVertexString) {
  CinderGraph<std::string, float> stringGraph;
  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_EQ(stringGraph.numVertices(), 3);

  EXPECT_TRUE(stringGraph.removeVertex("A"));
  EXPECT_TRUE(stringGraph.removeVertex("B"));
  EXPECT_FALSE(stringGraph.removeVertex("D"));

  EXPECT_EQ(stringGraph.numVertices(), 1);
}

TEST_F(CinderGraphFunctionalTest, RemoveVertexCustomType) {
  CinderGraph<ListVertex, float> customGraph;

  ListVertex v1(10);
  ListVertex v2(20);
  ListVertex v3(30);
  ListVertex v4(40);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  EXPECT_EQ(customGraph.numVertices(), 3);

  EXPECT_TRUE(customGraph.removeVertex(v1));
  EXPECT_TRUE(customGraph.removeVertex(v2));
  EXPECT_FALSE(customGraph.removeVertex(v4));

  EXPECT_EQ(customGraph.numVertices(), 1);
}
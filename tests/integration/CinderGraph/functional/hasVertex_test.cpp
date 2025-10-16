#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, HasVertexPrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);

  EXPECT_TRUE(intGraph.hasVertex(1));
  EXPECT_TRUE(intGraph.hasVertex(2));
  EXPECT_FALSE(intGraph.hasVertex(3)); // not added
}

TEST_F(CinderGraphFunctionalTest, HasVertexString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);

  EXPECT_TRUE(stringGraph.hasVertex("A"));
  EXPECT_TRUE(stringGraph.hasVertex("B"));
  EXPECT_FALSE(stringGraph.hasVertex("C")); // not added
}

TEST_F(CinderGraphFunctionalTest, HasVertexCustomType) {
  auto customGraph = builder.CreateCustomUnweightedGraph(GraphOpts::directed);

  ListVertex v1(10);
  ListVertex v2(20);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);

  EXPECT_TRUE(customGraph.hasVertex(v1));
  EXPECT_TRUE(customGraph.hasVertex(v2));
  EXPECT_FALSE(customGraph.hasVertex(ListVertex(30))); // not added
}
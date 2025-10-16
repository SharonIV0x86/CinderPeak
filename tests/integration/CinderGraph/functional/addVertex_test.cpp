#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, AddVertexPrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_FALSE(intGraph.addVertex(1).second); // duplicate
}

TEST_F(CinderGraphFunctionalTest, AddVertexString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_FALSE(stringGraph.addVertex("A").second); // duplicate
}

TEST_F(CinderGraphFunctionalTest, AddVertexCustomType) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(10);
  ListVertex v2(20);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_FALSE(customGraph.addVertex(v1).second); // duplicate
}
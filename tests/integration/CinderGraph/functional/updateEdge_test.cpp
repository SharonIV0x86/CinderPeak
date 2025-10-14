#include "../common/DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, UpdateEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 25).second);

  auto [weight, status] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 25);

  auto [new_weight, status1] = intGraph.updateEdge(1, 2, 50);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight, 50);

  EXPECT_FALSE(intGraph.updateEdge(2, 3, 100).second); // edge doesn't exist
}

TEST_F(CinderGraphFunctionalTest, UpdateEdgeString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B", 42).second);

  auto [weight, status] = stringGraph.getEdge("A", "B");
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 42);

  auto [new_weight, status1] = stringGraph.updateEdge("A", "B", 84);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight, 84);

  EXPECT_FALSE(
      stringGraph.updateEdge("B", "C", 100).second); // edge doesn't exist
}

TEST_F(CinderGraphFunctionalTest, UpdateCustomEdge) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(1), v2(2);
  ListEdge e1(3.5), e2(7.0f);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);

  auto [weight, status] = customGraph.getEdge(v1, v2);
  ASSERT_TRUE(status);
  EXPECT_EQ(weight->edge_weight, 3.5f);

  auto [new_weight, status1] = customGraph.updateEdge(v1, v2, e2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight.edge_weight, 7.0f);

  EXPECT_FALSE(customGraph.updateEdge(v2, v1, e1).second); // edge doesn't exist
}

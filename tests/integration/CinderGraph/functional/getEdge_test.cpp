#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, GetWeightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 15).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 25).second);

  auto [weight, status] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 5);

  auto [weight1, status1] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 25);

  auto [weight2, status2] = intGraph.getEdge(1, 6);
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, GetUnWeightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2).second);

  EXPECT_TRUE(intGraph.getEdge(1, 3).second);
  EXPECT_TRUE(intGraph.getEdge(1, 2).second);
  EXPECT_FALSE(intGraph.getEdge(1, 6).second);
}

TEST_F(CinderGraphFunctionalTest, GetWeightedEdgeString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B", 42).second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C", 100).second);

  auto [weight, status] = stringGraph.getEdge("A", "B");
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 42);

  auto [weight1, status1] = stringGraph.getEdge("B", "C");
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 100);

  auto [weight2, status2] = stringGraph.getEdge("A", "C");
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, GetUnweightedEdgeString) {
  auto stringGraph = builder.CreateStringUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B").second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C").second);

  EXPECT_TRUE(stringGraph.getEdge("A", "B").second);
  EXPECT_TRUE(stringGraph.getEdge("B", "C").second);
  EXPECT_FALSE(stringGraph.getEdge("A", "C").second);
}

TEST_F(CinderGraphFunctionalTest, GetCustomEdge) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5), e2(7.0);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(customGraph.addEdge(v2, v3, e2).second);

  auto [weight, status] = customGraph.getEdge(v1, v2);
  ASSERT_TRUE(status);
  EXPECT_EQ(weight->edge_weight, 3.5f);

  auto [weight2, status2] = customGraph.getEdge(v2, v3);
  ASSERT_TRUE(status2);
  EXPECT_EQ(weight2->edge_weight, 7.0f);

  auto [weight3, status3] = customGraph.getEdge(v1, v3);
  ASSERT_FALSE(status3);
  ASSERT_FALSE(weight3.has_value());
}

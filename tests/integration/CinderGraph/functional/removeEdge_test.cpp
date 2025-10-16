#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, RemoveWeightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 15).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 25).second);

  EXPECT_EQ(intGraph.numEdges(), 3);

  auto [weight, status] = intGraph.removeEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 5);

  auto [weight1, status1] = intGraph.removeEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 25);

  EXPECT_EQ(intGraph.numEdges(), 1);

  auto [weight2, status2] = intGraph.removeEdge(1, 6);
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, RemoveUnWeightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);
  EXPECT_TRUE(intGraph.addEdge(1, 3).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2).second);

  EXPECT_EQ(intGraph.numEdges(), 3);

  EXPECT_TRUE(intGraph.removeEdge(1, 3).second);
  EXPECT_TRUE(intGraph.removeEdge(1, 2).second);

  EXPECT_EQ(intGraph.numEdges(), 1);

  EXPECT_FALSE(intGraph.removeEdge(1, 6).second);
}

TEST_F(CinderGraphFunctionalTest, RemoveWeightedEdgeString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B", 42).second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C", 100).second);

  EXPECT_EQ(stringGraph.numEdges(), 2);

  auto [weight, status] = stringGraph.removeEdge("A", "B");
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 42);

  auto [weight1, status1] = stringGraph.removeEdge("B", "C");
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 100);

  EXPECT_EQ(stringGraph.numEdges(), 0);

  auto [weight2, status2] = stringGraph.removeEdge("A", "C");
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, RemoveUnweightedEdgeString) {
  auto stringGraph = builder.CreateStringUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B").second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C").second);

  EXPECT_EQ(stringGraph.numEdges(), 2);

  EXPECT_TRUE(stringGraph.removeEdge("A", "B").second);
  EXPECT_TRUE(stringGraph.removeEdge("B", "C").second);

  EXPECT_EQ(stringGraph.numEdges(), 0);

  EXPECT_FALSE(stringGraph.removeEdge("A", "C").second);
}

TEST_F(CinderGraphFunctionalTest, RemoveCustomEdge) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5), e2(7.0);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(customGraph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(customGraph.numEdges(), 2);

  auto [weight, status] = customGraph.removeEdge(v1, v2);
  ASSERT_TRUE(status);
  EXPECT_EQ(weight->edge_weight, 3.5f);

  auto [weight2, status2] = customGraph.removeEdge(v2, v3);
  ASSERT_TRUE(status2);
  EXPECT_EQ(weight2->edge_weight, 7.0f);

  auto [weight3, status3] = customGraph.removeEdge(v1, v3);
  ASSERT_FALSE(status3);
  ASSERT_FALSE(weight3.has_value());

  EXPECT_EQ(customGraph.numEdges(), 0);
}

#include "../common/DummyGraphBuilder.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class ClearAndReuseGraphTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(ClearAndReuseGraphTest, ClearGraph) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 1).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 2).second);
  EXPECT_TRUE(intGraph.addEdge(3, 1, 3).second);

  EXPECT_EQ(intGraph.numEdges(), 3);
  EXPECT_EQ(intGraph.numVertices(), 3);

  intGraph.clearEdges();

  EXPECT_EQ(intGraph.numEdges(), 0);
  EXPECT_EQ(intGraph.numVertices(), 3);

  EXPECT_TRUE(intGraph.hasVertex(1));
  EXPECT_TRUE(intGraph.hasVertex(2));
  EXPECT_TRUE(intGraph.hasVertex(3));

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);
  EXPECT_FALSE(intGraph.getEdge(2, 3).second);
  EXPECT_FALSE(intGraph.getEdge(3, 1).second);
}

TEST_F(ClearAndReuseGraphTest, ClearAndReuseGraph) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 10).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 20).second);

  EXPECT_EQ(intGraph.numEdges(), 2);

  intGraph.clearEdges();

  EXPECT_EQ(intGraph.numEdges(), 0);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 30).second);
  EXPECT_TRUE(intGraph.addEdge(3, 2, 40).second);

  EXPECT_EQ(intGraph.numEdges(), 2);

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);
  EXPECT_FALSE(intGraph.getEdge(2, 3).second);

  auto [weight, status] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 30);

  auto [weight1, status1] = intGraph.getEdge(3, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 40);

  EXPECT_TRUE(intGraph.addVertex(4).second);
  EXPECT_TRUE(intGraph.addVertex(5).second);

  intGraph.clearEdges();

  EXPECT_EQ(intGraph.numVertices(), 5);
  EXPECT_EQ(intGraph.numEdges(), 0);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 10).second);
  EXPECT_TRUE(intGraph.addEdge(2, 4, 20).second);
  EXPECT_TRUE(intGraph.addEdge(3, 5, 30).second);

  EXPECT_EQ(intGraph.numEdges(), 3);

  auto [weight2, status2] = intGraph.getEdge(3, 5);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 30);

  auto [weight3, status3] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 10);

  auto [weight4, status4] = intGraph.getEdge(2, 4);
  EXPECT_TRUE(status4);
  EXPECT_EQ(weight4, 20);

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);
}

TEST_F(ClearAndReuseGraphTest, ClearAndReuseUnweightedGraph) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3).second);

  EXPECT_EQ(intGraph.numEdges(), 2);

  intGraph.clearEdges();
  EXPECT_EQ(intGraph.numEdges(), 0);

  EXPECT_TRUE(intGraph.addEdge(1, 3).second);
  EXPECT_TRUE(intGraph.addEdge(3, 2).second);

  EXPECT_EQ(intGraph.numEdges(), 2);

  EXPECT_TRUE(intGraph.getEdge(1, 3).second);
  EXPECT_TRUE(intGraph.getEdge(3, 2).second);

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);
  EXPECT_FALSE(intGraph.getEdge(2, 3).second);
}

TEST_F(ClearAndReuseGraphTest, StressTest) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  for (int weight = 1; weight <= 100; weight++) {
    EXPECT_TRUE(intGraph.addEdge(1, 2, weight).second);
    EXPECT_TRUE(intGraph.addEdge(2, 3, weight * 2).second);
    EXPECT_TRUE(intGraph.addEdge(3, 1, weight * 3).second);

    EXPECT_EQ(intGraph.numEdges(), 3);

    auto [weight1, status1] = intGraph.getEdge(1, 2);
    EXPECT_TRUE(status1);
    EXPECT_EQ(weight1, weight);

    intGraph.clearEdges();
    EXPECT_EQ(intGraph.numEdges(), 0);
  }

  EXPECT_EQ(intGraph.numVertices(), 3);
  EXPECT_TRUE(intGraph.hasVertex(1));
  EXPECT_TRUE(intGraph.hasVertex(2));
  EXPECT_TRUE(intGraph.hasVertex(3));
}

TEST_F(ClearAndReuseGraphTest, ClearEmptyGraph) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);

  EXPECT_EQ(intGraph.numEdges(), 0);

  intGraph.clearEdges();

  EXPECT_EQ(intGraph.numEdges(), 0);
  EXPECT_EQ(intGraph.numVertices(), 2);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 5).second);
  EXPECT_EQ(intGraph.numEdges(), 1);
}
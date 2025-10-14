#include "../common/DummyGraphBuilder.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class BasicConnectivityTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(BasicConnectivityTest, AddVerticesAndEdges) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_EQ(intGraph.numVertices(), 3);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 1).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 2).second);

  EXPECT_EQ(intGraph.numEdges(), 2);

  auto [weight1, status1] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 1);

  auto [weight2, status2] = intGraph.getEdge(2, 3);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 2);
}

TEST_F(BasicConnectivityTest, RemoveEdges) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 1).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 2).second);
  EXPECT_TRUE(intGraph.addEdge(1, 3, 3).second);

  EXPECT_EQ(intGraph.numEdges(), 3);

  auto [weight, status] = intGraph.removeEdge(2, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 2);

  EXPECT_EQ(intGraph.numEdges(), 2);
  EXPECT_FALSE(intGraph.getEdge(2, 3).second);

  auto [weight1, status1] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 1);

  auto [weight2, status2] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 3);
}

TEST_F(BasicConnectivityTest, RemoveVertices) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  intGraph.addVertex(1);
  intGraph.addVertex(2);
  intGraph.addVertex(3);

  intGraph.addEdge(1, 2, 1);
  intGraph.addEdge(2, 3, 2);
  intGraph.addEdge(1, 3, 3);

  EXPECT_TRUE(intGraph.removeVertex(2));

  EXPECT_FALSE(intGraph.hasVertex(2));

  auto [weight, status] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 3);
}

TEST_F(BasicConnectivityTest, ComplexGraph) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  intGraph.addVertex(1);
  intGraph.addVertex(2);
  intGraph.addVertex(3);

  intGraph.addEdge(1, 2, 1);
  intGraph.addEdge(2, 3, 2);
  intGraph.addEdge(3, 1, 3);

  EXPECT_EQ(intGraph.numEdges(), 3);

  intGraph.addEdge(1, 3, 4);

  EXPECT_EQ(intGraph.numEdges(), 4);

  auto [weight, status] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 1);

  auto [weight2, status2] = intGraph.getEdge(3, 1);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 3);

  auto [weight3, status3] = intGraph.getEdge(1, 3);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 4);

  intGraph.removeVertex(1);
  EXPECT_EQ(intGraph.numVertices(), 2);

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);
  EXPECT_FALSE(intGraph.getEdge(3, 1).second);
  EXPECT_TRUE(intGraph.getEdge(2, 3).second);
}

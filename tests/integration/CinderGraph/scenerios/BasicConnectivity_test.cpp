#include "CinderGraph.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class BasicConnectivityTest : public ::testing::Test {
protected:
  void SetUp() override { CinderGraph<int, double>::setConsoleLogging(false); }
};

TEST_F(BasicConnectivityTest, AddVerticesAndEdges) {
  CinderGraph<int, double> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_EQ(graph.numVertices(), 3);

  EXPECT_TRUE(graph.addEdge(1, 2, 1.5).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 2.5).second);

  EXPECT_EQ(graph.numEdges(), 2);

  auto [weight1, status1] = graph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 1.5);

  auto [weight2, status2] = graph.getEdge(2, 3);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 2.5);
}

TEST_F(BasicConnectivityTest, RemoveEdges) {
  CinderGraph<int, double> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 1.0).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 2.0).second);
  EXPECT_TRUE(graph.addEdge(1, 3, 3.0).second);

  EXPECT_EQ(graph.numEdges(), 3);

  auto [weight, status] = graph.removeEdge(2, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 2.0);

  EXPECT_EQ(graph.numEdges(), 2);
  EXPECT_FALSE(graph.getEdge(2, 3).second);

  auto [weight1, status1] = graph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 1.0);

  auto [weight2, status2] = graph.getEdge(1, 3);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 3.0);
}

TEST_F(BasicConnectivityTest, RemoveVertices) {
  CinderGraph<int, double> graph;

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  graph.addEdge(1, 2, 1.0);
  graph.addEdge(2, 3, 2.0);
  graph.addEdge(1, 3, 3.0);

  EXPECT_TRUE(graph.removeVertex(2));

  EXPECT_FALSE(graph.hasVertex(2));

  auto [weight, status] = graph.getEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 3.0);
}

TEST_F(BasicConnectivityTest, ComplexGraph) {
  CinderGraph<int, double> graph;

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  graph.addEdge(1, 2, 1.0);
  graph.addEdge(2, 3, 2.0);
  graph.addEdge(3, 1, 3.0);

  EXPECT_EQ(graph.numEdges(), 3);

  graph.addEdge(1, 3, 4.0);

  EXPECT_EQ(graph.numEdges(), 4);

  auto [weight, status] = graph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 1.0);

  auto [weight2, status2] = graph.getEdge(3, 1);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 3.0);

  auto [weight3, status3] = graph.getEdge(1, 3);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 4.0);

  graph.removeVertex(1);
  EXPECT_EQ(graph.numVertices(), 2);

  EXPECT_FALSE(graph.getEdge(1, 2).second);
  EXPECT_FALSE(graph.getEdge(3, 1).second);
  EXPECT_TRUE(graph.getEdge(2, 3).second);
}

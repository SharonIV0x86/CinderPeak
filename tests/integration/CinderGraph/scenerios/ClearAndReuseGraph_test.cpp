#include "CinderGraph.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class ClearAndReuseGraphTest : public ::testing::Test {
protected:
  void SetUp() override { CinderGraph<int, int>::setConsoleLogging(false); }
};

TEST_F(ClearAndReuseGraphTest, ClearGraph) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 1).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 2).second);
  EXPECT_TRUE(graph.addEdge(3, 1, 3).second);

  EXPECT_EQ(graph.numEdges(), 3);
  EXPECT_EQ(graph.numVertices(), 3);

  graph.clearEdges();

  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 3);

  EXPECT_TRUE(graph.hasVertex(1));
  EXPECT_TRUE(graph.hasVertex(2));
  EXPECT_TRUE(graph.hasVertex(3));

  EXPECT_FALSE(graph.getEdge(1, 2).second);
  EXPECT_FALSE(graph.getEdge(2, 3).second);
  EXPECT_FALSE(graph.getEdge(3, 1).second);
}

TEST_F(ClearAndReuseGraphTest, ClearAndReuseGraph) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 10).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 20).second);

  EXPECT_EQ(graph.numEdges(), 2);

  graph.clearEdges();

  EXPECT_EQ(graph.numEdges(), 0);

  EXPECT_TRUE(graph.addEdge(1, 3, 30).second);
  EXPECT_TRUE(graph.addEdge(3, 2, 40).second);

  EXPECT_EQ(graph.numEdges(), 2);

  EXPECT_FALSE(graph.getEdge(1, 2).second);
  EXPECT_FALSE(graph.getEdge(2, 3).second);

  auto [weight, status] = graph.getEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 30);

  auto [weight1, status1] = graph.getEdge(3, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 40);

  EXPECT_TRUE(graph.addVertex(4).second);
  EXPECT_TRUE(graph.addVertex(5).second);

  graph.clearEdges();

  EXPECT_EQ(graph.numVertices(), 5);
  EXPECT_EQ(graph.numEdges(), 0);

  EXPECT_TRUE(graph.addEdge(1, 3, 10).second);
  EXPECT_TRUE(graph.addEdge(2, 4, 20).second);
  EXPECT_TRUE(graph.addEdge(3, 5, 30).second);

  EXPECT_EQ(graph.numEdges(), 3);

  auto [weight2, status2] = graph.getEdge(3, 5);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 30);

  auto [weight3, status3] = graph.getEdge(1, 3);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 10);

  auto [weight4, status4] = graph.getEdge(2, 4);
  EXPECT_TRUE(status4);
  EXPECT_EQ(weight4, 20);

  EXPECT_FALSE(graph.getEdge(1, 2).second);
}

TEST_F(ClearAndReuseGraphTest, ClearAndReuseUnweightedGraph) {
  CinderGraph<int, Unweighted> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 2).second);
  EXPECT_TRUE(graph.addEdge(2, 3).second);

  EXPECT_EQ(graph.numEdges(), 2);

  graph.clearEdges();
  EXPECT_EQ(graph.numEdges(), 0);

  EXPECT_TRUE(graph.addEdge(1, 3).second);
  EXPECT_TRUE(graph.addEdge(3, 2).second);

  EXPECT_EQ(graph.numEdges(), 2);

  EXPECT_TRUE(graph.getEdge(1, 3).second);
  EXPECT_TRUE(graph.getEdge(3, 2).second);

  EXPECT_FALSE(graph.getEdge(1, 2).second);
  EXPECT_FALSE(graph.getEdge(2, 3).second);
}

TEST_F(ClearAndReuseGraphTest, StressTest) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  for (int weight = 1; weight <= 100; weight++) {
    EXPECT_TRUE(graph.addEdge(1, 2, weight).second);
    EXPECT_TRUE(graph.addEdge(2, 3, weight * 2).second);
    EXPECT_TRUE(graph.addEdge(3, 1, weight * 3).second);

    EXPECT_EQ(graph.numEdges(), 3);

    auto [weight1, status1] = graph.getEdge(1, 2);
    EXPECT_TRUE(status1);
    EXPECT_EQ(weight1, weight);

    graph.clearEdges();
    EXPECT_EQ(graph.numEdges(), 0);
  }

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_TRUE(graph.hasVertex(1));
  EXPECT_TRUE(graph.hasVertex(2));
  EXPECT_TRUE(graph.hasVertex(3));
}

TEST_F(ClearAndReuseGraphTest, ClearEmptyGraph) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);

  EXPECT_EQ(graph.numEdges(), 0);

  graph.clearEdges();

  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 2);

  EXPECT_TRUE(graph.addEdge(1, 2, 5).second);
  EXPECT_EQ(graph.numEdges(), 1);
}
#include "CinderPeak.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class ListVertex : public CinderVertex {
public:
  int data;
  ListVertex(int d = 0) : data(d) {}
  bool operator==(const ListVertex &other) const { return data == other.data; }
};

class ListEdge : public CinderEdge {
public:
  float edge_weight;
  ListEdge(float w = 0.0f) : edge_weight(w) {}
  bool operator==(const ListEdge &other) const {
    return edge_weight == other.edge_weight;
  }
};

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  GraphCreationOptions directedOpts;
  GraphCreationOptions undirectedOpts;

  CinderGraphFunctionalTest()
      : directedOpts({GraphCreationOptions::Directed}),
        undirectedOpts({GraphCreationOptions::Undirected}) {}
};

TEST_F(CinderGraphFunctionalTest, ClearWeightedEdges) {
  CinderGraph<int, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 15).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 2);

  graph.clearEdges();
  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 3); // vertices remain after clear
}

TEST_F(CinderGraphFunctionalTest, ClearUnweightedEdges) {
  CinderGraph<int, Unweighted> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 2).second);
  EXPECT_TRUE(graph.addEdge(2, 3).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 2);

  graph.clearEdges();
  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 3); // vertices remain after clear
}

TEST_F(CinderGraphFunctionalTest, ClearEdgesCustomTypes) {
  CinderGraph<ListVertex, ListEdge> graph(directedOpts);

  ListVertex v1(10);
  ListVertex v2(20);
  ListVertex v3(30);

  EXPECT_TRUE(graph.addVertex(v1).second);
  EXPECT_TRUE(graph.addVertex(v2).second);
  EXPECT_TRUE(graph.addVertex(v3).second);

  ListEdge e1(1.5f);
  ListEdge e2(2.5f);

  EXPECT_TRUE(graph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(graph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 2);

  graph.clearEdges();

  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 3); // vertices remain after clear
}

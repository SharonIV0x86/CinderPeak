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

TEST_F(CinderGraphFunctionalTest, AddWeightedEdgePrimitive) {
  CinderGraph<int, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 15).second);
  EXPECT_TRUE(graph.addEdge(1, 2, 25).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddUnweightedEdgePrimitive) {
  CinderGraph<int, Unweighted> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 3).second);
  EXPECT_TRUE(graph.addEdge(2, 3).second);
  EXPECT_TRUE(graph.addEdge(1, 2).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddWeightedEdgeString) {
  CinderGraph<std::string, float> stringGraph(directedOpts);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "C", 2.5f).second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C", 7.5f).second);
  EXPECT_TRUE(stringGraph.addEdge("A", "B", 12.5f).second);

  EXPECT_EQ(stringGraph.numVertices(), 3);
  EXPECT_EQ(stringGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddUnWeightedEdgeString) {
  CinderGraph<std::string, Unweighted> stringGraph(directedOpts);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "C").second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C").second);
  EXPECT_TRUE(stringGraph.addEdge("A", "B").second);

  EXPECT_EQ(stringGraph.numVertices(), 3);
  EXPECT_EQ(stringGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddCustomVertexAndEdge) {
  CinderPeak::CinderGraph<ListVertex, ListEdge> graph;

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5), e2(7.0);

  EXPECT_TRUE(graph.addVertex(v1).second);
  EXPECT_TRUE(graph.addVertex(v2).second);
  EXPECT_TRUE(graph.addVertex(v3).second);

  EXPECT_TRUE(graph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(graph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 2);
}

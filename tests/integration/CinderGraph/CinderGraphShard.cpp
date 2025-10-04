#include "CinderPeak.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

// ---------------- Custom Types ----------------
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

// ---------------- Test Fixture ----------------
class CinderGraphTest : public ::testing::Test {
protected:
  GraphCreationOptions directedOpts;
  GraphCreationOptions undirectedOpts;

  CinderGraphTest()
      : directedOpts({GraphCreationOptions::Directed}),
        undirectedOpts({GraphCreationOptions::Undirected}) {}
};

// Test 1: Weighted int edges (add, update, clear)
TEST_F(CinderGraphTest, WeightedEdgesAddUpdateClear) {
  CinderGraph<int, int> graph(directedOpts);
  CinderGraph<int, int>::setConsoleLogging(false);

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  auto [e1, added1] = graph.addEdge(1, 3, 5);
  EXPECT_TRUE(added1);

  auto [newW, updated] = graph.updateEdge(1, 3, 10);
  EXPECT_TRUE(updated);
  EXPECT_EQ(newW, 10);

  EXPECT_EQ(graph.numVertices(), 3);
  EXPECT_EQ(graph.numEdges(), 1);

  graph.clearEdges();
  EXPECT_EQ(graph.numEdges(), 0);
  EXPECT_EQ(graph.numVertices(), 3); // vertices remain after clear
}

// Test 2: Unweighted graph (edge insertion and vertex count)
TEST_F(CinderGraphTest, UnweightedGraphEdges) {
  CinderGraph<int, Unweighted> g(directedOpts);

  g.addVertex(1);
  g.addVertex(2);
  g.addVertex(3);

  auto [edgeKey, inserted] = g.addEdge(1, 2);
  EXPECT_TRUE(inserted);
  EXPECT_EQ(edgeKey.first, 1);
  EXPECT_EQ(edgeKey.second, 2);

  EXPECT_TRUE(g.hasVertex(2));
  EXPECT_FALSE(g.hasVertex(99));

  EXPECT_EQ(g.numVertices(), 3);
  EXPECT_EQ(g.numEdges(), 1);
}
TEST_F(CinderGraphTest, CustomVertexAndEdge) {
  CinderPeak::CinderGraph<ListVertex, ListEdge> g;

  ListVertex v1(1), v2(2);
  ListEdge e1(3.5), e2(7.0);

  g.addVertex(v1);
  g.addVertex(v2);

  g.addEdge(v1, v2, e1);

  // updateEdge now returns pair<newWeight, success>
  auto [newEdge, updated] = g.updateEdge(v1, v2, e2);

  EXPECT_TRUE(updated);
  EXPECT_EQ(newEdge.edge_weight, 7.0);

  // getEdge returns pair<optional<Edge>, bool>
  auto [maybeEdge, ok] = g.getEdge(v1, v2);
  ASSERT_TRUE(ok);
  ASSERT_TRUE(maybeEdge.has_value());
  EXPECT_EQ(maybeEdge->edge_weight, 7.0);
}

#include "../../../src/CinderGraph.hpp"
#include <gtest/gtest.h>

using namespace CinderPeak;

TEST(IteratorTest, VerticesMatchCount) {
  CinderGraph<int, int> g(
      GraphCreationOptions({GraphCreationOptions::Directed}));
  g.addVertex(1);
  g.addVertex(2);
  g.addVertex(3);

  auto verts = g.vertices();
  EXPECT_EQ(verts.size(), g.numVertices());
}

TEST(IteratorTest, VerticesContainsAdded) {
  CinderGraph<int, int> g(
      GraphCreationOptions({GraphCreationOptions::Directed}));
  g.addVertex(10);
  g.addVertex(20);

  auto verts = g.vertices();
  EXPECT_TRUE(std::find(verts.begin(), verts.end(), 10) != verts.end());
  EXPECT_TRUE(std::find(verts.begin(), verts.end(), 20) != verts.end());
}

TEST(IteratorTest, EdgesMatchCount) {
  CinderGraph<int, int> g(
      GraphCreationOptions({GraphCreationOptions::Directed}));
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 99);

  auto edgeList = g.edges();
  EXPECT_EQ(edgeList.size(), g.numEdges());
}

TEST(IteratorTest, EdgesContainsAdded) {
  CinderGraph<int, int> g(
      GraphCreationOptions({GraphCreationOptions::Directed}));
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 42);

  auto edgeList = g.edges();
  bool found = false;
  for (auto &[src, dest, w] : edgeList) {
    if (src == 1 && dest == 2 && w == 42)
      found = true;
  }
  EXPECT_TRUE(found);
}

TEST(IteratorTest, EmptyGraphReturnsEmpty) {
  CinderGraph<int, int> g(
      GraphCreationOptions({GraphCreationOptions::Directed}));
  EXPECT_TRUE(g.vertices().empty());
  EXPECT_TRUE(g.edges().empty());
}
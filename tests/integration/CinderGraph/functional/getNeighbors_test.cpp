#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"
#include <algorithm>

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, GetNeighborsPrimitiveWeighted) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 10).second);
  EXPECT_TRUE(intGraph.addEdge(1, 3, 20).second);

  auto neighbors = intGraph.getNeighbors(1);
  EXPECT_EQ(neighbors.size(), 2u);
  auto has = [&](int v, int w) {
    return std::find(neighbors.begin(), neighbors.end(),
                     std::make_pair(v, w)) != neighbors.end();
  };
  EXPECT_TRUE(has(2, 10));
  EXPECT_TRUE(has(3, 20));
}

TEST_F(CinderGraphFunctionalTest, GetNeighborsPrimitiveUndirected) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::undirected);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 5).second);

  // Undirected: both vertices see each other as a neighbor
  auto neighbors1 = intGraph.getNeighbors(1);
  auto neighbors2 = intGraph.getNeighbors(2);

  EXPECT_EQ(neighbors1.size(), 1u);
  EXPECT_EQ(neighbors2.size(), 1u);

  EXPECT_EQ(neighbors1[0].first, 2);
  EXPECT_EQ(neighbors1[0].second, 5);
  EXPECT_EQ(neighbors2[0].first, 1);
  EXPECT_EQ(neighbors2[0].second, 5);
}

TEST_F(CinderGraphFunctionalTest, GetNeighborsStringWeighted) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "B", 1.5f).second);
  EXPECT_TRUE(stringGraph.addEdge("A", "C", 2.5f).second);

  auto neighbors = stringGraph.getNeighbors("A");
  EXPECT_EQ(neighbors.size(), 2u);

  auto has = [&](const std::string &v, float w) {
    return std::find(neighbors.begin(), neighbors.end(),
                     std::make_pair(v, w)) != neighbors.end();
  };
  EXPECT_TRUE(has("B", 1.5f));
  EXPECT_TRUE(has("C", 2.5f));
}

TEST_F(CinderGraphFunctionalTest, GetNeighborsCustomWeighted) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5f), e2(7.0f);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(customGraph.addEdge(v1, v3, e2).second);

  auto neighbors = customGraph.getNeighbors(v1);
  EXPECT_EQ(neighbors.size(), 2u);

  auto has = [&](const ListVertex &v, const ListEdge &e) {
    return std::find(neighbors.begin(), neighbors.end(),
                     std::make_pair(v, e)) != neighbors.end();
  };
  EXPECT_TRUE(has(v2, e1));
  EXPECT_TRUE(has(v3, e2));
}

TEST_F(CinderGraphFunctionalTest, GetNeighborsVertexWithNoEdges) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 10).second);

  // Vertex 2 exists but has no outgoing edges in a directed graph
  EXPECT_TRUE(intGraph.getNeighbors(2).empty());
}

TEST_F(CinderGraphFunctionalTest, GetNeighborsNonExistentVertex) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 10).second);

  // Vertex 99 was never added — should return empty, not crash
  EXPECT_TRUE(intGraph.getNeighbors(99).empty());
}

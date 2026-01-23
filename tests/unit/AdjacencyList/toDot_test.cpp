#include "AdjacencyListTestBase.hpp"
#include "gtest/gtest.h"

TEST_F(AdjacencyStorageShardTest, ToDotDirectedGraph) {
  intGraph.impl_clearVertices(); // Start fresh to ensure IDs are 1, 2, 3
  EXPECT_TRUE(intGraph.impl_addVertex(1).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(2).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(3).isOK());

  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 100).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 200).isOK());

  std::string dot = intGraph.impl_toDot(true);

  EXPECT_NE(dot.find("digraph"), std::string::npos);
  // With cleared graph, IDs start at 1
  EXPECT_NE(dot.find("node_1 [label=\"1\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_1 -> node_2 [label=\"100\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_2 -> node_3 [label=\"200\"]"), std::string::npos);
}

TEST_F(AdjacencyStorageShardTest, ToDotUndirectedGraph) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(1).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(2).isOK());

  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 50).isOK());

  std::string dot = intGraph.impl_toDot(false);

  EXPECT_NE(dot.find("graph"), std::string::npos);
  EXPECT_NE(dot.find("--"), std::string::npos);
  EXPECT_NE(dot.find("node_1 -- node_2 [label=\"50\"]"), std::string::npos);
}

TEST_F(AdjacencyStorageShardTest, ToDotIsolatedNodes) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(10).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(20).isOK());

  std::string dot = intGraph.impl_toDot(true);

  // 1st vertex added => ID 1, 2nd => ID 2
  EXPECT_NE(dot.find("node_1 [label=\"10\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_2 [label=\"20\"]"), std::string::npos);
}

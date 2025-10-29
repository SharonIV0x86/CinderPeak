#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, RemoveExistingVertex) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(3, 1, 10);

  // Remove vertex 1
  auto status = intGraph.impl_removeVertex(1);
  EXPECT_TRUE(status.isOK());

  // Ensure vertex 1 is removed
  auto neighbors = intGraph.impl_getNeighbors(1);
  EXPECT_FALSE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);

  auto neighbors3 = intGraph.impl_getNeighbors(3);
  EXPECT_TRUE(neighbors3.second.isOK());
  for (auto &edge : neighbors3.first) {
    EXPECT_NE(edge.first, 1);
  }

  auto neighbors2 = intGraph.impl_getNeighbors(2);
  EXPECT_TRUE(neighbors2.second.isOK());
  for (auto &edge : neighbors2.first) {
    EXPECT_NE(edge.first, 1);
  }
}

TEST_F(AdjacencyStorageShardTest, RemoveNonExistentVertex) {
  auto status = intGraph.impl_removeVertex(999);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_NOT_FOUND);
}

TEST_F(AdjacencyStorageShardTest, RemoveVertexFromStringGraph) {
  stringGraph.impl_addEdge("A", "B", 1.5f);

  auto status = stringGraph.impl_removeVertex("A");
  EXPECT_TRUE(status.isOK());

  auto neighbors = stringGraph.impl_getNeighbors("A");
  EXPECT_FALSE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);

  auto neighborsB = stringGraph.impl_getNeighbors("B");
  EXPECT_TRUE(neighborsB.second.isOK());
  for (auto &edge : neighborsB.first) {
    EXPECT_NE(edge.first, "A");
  }
}
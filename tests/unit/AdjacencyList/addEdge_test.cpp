#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, AddEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 5);

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 10);
}

TEST_F(AdjacencyStorageShardTest, AddEdgeWithoutWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());

  auto edge = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_EQ(edge.first, 0); // Default int
}

TEST_F(AdjacencyStorageShardTest, AddEdgeInvalidVertices) {
  auto status1 = intGraph.impl_addEdge(99, 1);
  EXPECT_FALSE(status1.isOK());
  EXPECT_EQ(status1.code(), StatusCode::VERTEX_NOT_FOUND);

  auto status2 = intGraph.impl_addEdge(1, 99);
  EXPECT_FALSE(status2.isOK());
  EXPECT_EQ(status2.code(), StatusCode::VERTEX_NOT_FOUND);
}
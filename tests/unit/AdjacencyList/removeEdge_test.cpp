#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, RemoveEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());

  auto result1 = intGraph.impl_removeEdge(1, 2);
  EXPECT_TRUE(result1.second.isOK());
  EXPECT_EQ(result1.first, edge1.first);
  edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_FALSE(edge1.second.isOK());

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());

  auto result2 = intGraph.impl_removeEdge(2, 3);
  EXPECT_TRUE(result2.second.isOK());
  EXPECT_EQ(result2.first, edge2.first);
  edge2 = intGraph.impl_getEdge(1, 2);
  EXPECT_FALSE(edge2.second.isOK());

  EXPECT_FALSE(intGraph.impl_removeEdge(5, 6).second.isOK());
}

TEST_F(AdjacencyStorageShardTest, RemoveEdgeWithoutWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());

  EXPECT_TRUE(intGraph.impl_removeEdge(1, 2).second.isOK());
  edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_FALSE(edge1.second.isOK());

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());

  EXPECT_TRUE(intGraph.impl_removeEdge(2, 3).second.isOK());
  edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_FALSE(edge2.second.isOK());

  EXPECT_FALSE(intGraph.impl_removeEdge(5, 6).second.isOK());
}
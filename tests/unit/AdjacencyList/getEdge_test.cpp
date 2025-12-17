#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, GetExistingEdge) {
  intGraph.impl_addEdge(1, 2, 5);

  auto result = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(result.second.isOK());
  EXPECT_EQ(result.first, 5);
}

TEST_F(AdjacencyStorageShardTest, GetNonExistentEdge) {
  auto result1 = intGraph.impl_getEdge(1, 3);
  EXPECT_FALSE(result1.second.isOK());
  EXPECT_EQ(result1.second.code(), StatusCode::EDGE_NOT_FOUND);

  auto result2 = intGraph.impl_getEdge(99, 1);
  EXPECT_FALSE(result2.second.isOK());
  EXPECT_EQ(result2.second.code(), StatusCode::VERTEX_NOT_FOUND);
}

TEST_F(AdjacencyStorageShardTest, GetNeighbors) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(1, 3, 10);

  auto neighbors = intGraph.impl_getNeighbors(1);
  EXPECT_TRUE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.first.size(), 2);

  EXPECT_EQ(neighbors.first[0].first, 2);
  EXPECT_EQ(neighbors.first[0].second, 5);

  EXPECT_EQ(neighbors.first[1].first, 3);
  EXPECT_EQ(neighbors.first[1].second, 10);
}

TEST_F(AdjacencyStorageShardTest, GetNeighborsNonExistentVertex) {
  auto neighbors = intGraph.impl_getNeighbors(99);
  EXPECT_FALSE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);
  EXPECT_TRUE(neighbors.first.empty());
}

TEST_F(AdjacencyStorageShardTest, EdgeExistence) {
  intGraph.impl_addEdge(1, 2, 5);

  auto edge1_2 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1_2.second.isOK());

  auto edge1_3 = intGraph.impl_getEdge(1, 3);
  EXPECT_FALSE(edge1_3.second.isOK());

  auto edge2_1 = intGraph.impl_getEdge(2, 1);
  EXPECT_FALSE(edge2_1.second.isOK());

  auto edge99_1 = intGraph.impl_getEdge(99, 1);
  EXPECT_FALSE(edge99_1.second.isOK());
}

// TEST_F(AdjacencyStorageShardTest, AdjacencyListStructure) {
//   intGraph.impl_addEdge(1, 2, 5);
//   intGraph.impl_addEdge(1, 3, 10);
//   intGraph.impl_addEdge(2, 3, 15);
//   intGraph.impl_addEdge(4, 4, 4);
//   intGraph.impl_addEdge(2, 5, 1);

//   auto adjList = intGraph.getAdjList();

//   EXPECT_EQ(adjList.size(), 8);

//   auto it1 = adjList.find(1);
//   ASSERT_NE(it1, adjList.end());
//   EXPECT_EQ(it1->second.size(), 2);

//   auto it2 = adjList.find(2);
//   ASSERT_NE(it2, adjList.end());
//   EXPECT_EQ(it2->second.size(), 2);

//   auto it3 = adjList.find(3);
//   ASSERT_NE(it3, adjList.end());
//   EXPECT_TRUE(it3->second.empty());

//   auto it4 = adjList.find(4);
//   ASSERT_NE(it4, adjList.end());
//   EXPECT_EQ(it4->second.size(), 1);

//   auto it5 = adjList.find(5);
//   ASSERT_NE(it5, adjList.end());
//   EXPECT_TRUE(it5->second.empty());
// }
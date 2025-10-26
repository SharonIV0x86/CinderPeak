#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, AddEdgesPairs) {
  std::vector<std::pair<int, int>> edges = {
      {1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  for (const auto &edge : edges) {
    auto result = intGraph.impl_getEdge(edge.first, edge.second);
    EXPECT_TRUE(result.second.isOK());
    EXPECT_EQ(result.first, 0);
  }
}

TEST_F(AdjacencyStorageShardTest, AddEdgesTuples) {
  std::vector<std::tuple<int, int, int>> edges = {
      {1, 2, 10}, {2, 3, 20}, {3, 4, 30}, {4, 5, 40}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  EXPECT_EQ(intGraph.impl_getEdge(1, 2).first, 10);
  EXPECT_EQ(intGraph.impl_getEdge(2, 3).first, 20);
  EXPECT_EQ(intGraph.impl_getEdge(3, 4).first, 30);
  EXPECT_EQ(intGraph.impl_getEdge(4, 5).first, 40);
}

TEST_F(AdjacencyStorageShardTest, AddEdgesInvalidVertices) {
  std::vector<std::pair<int, int>> edgesWithInvalid = {
      {1, 2}, {99, 3}, {4, 5}, {1, 100}};

  auto status = intGraph.impl_addEdges(edgesWithInvalid);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_NOT_FOUND);

  EXPECT_TRUE(intGraph.impl_getEdge(1, 2).second.isOK());
  EXPECT_TRUE(intGraph.impl_getEdge(4, 5).second.isOK());

  EXPECT_FALSE(intGraph.impl_getEdge(99, 3).second.isOK());
  EXPECT_FALSE(intGraph.impl_getEdge(1, 100).second.isOK());
}

TEST_F(AdjacencyStorageShardTest, AddEdgesEmpty) {
  std::vector<std::pair<int, int>> emptyEdges = {};

  auto status = intGraph.impl_addEdges(emptyEdges);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyStorageShardTest, AddEdgesMixedTypes) {
  std::vector<std::tuple<std::string, std::string, float>> edges = {
      {"A", "B", 1.5f}, {"B", "C", 2.7f}, {"A", "C", 3.14f}};

  auto status = stringGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("A", "B").first, 1.5f);
  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("B", "C").first, 2.7f);
  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("A", "C").first, 3.14f);
}
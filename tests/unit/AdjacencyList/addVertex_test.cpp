#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, AddVertexPrimitive) {
  EXPECT_TRUE(intGraph.impl_addVertex(6).isOK());

  auto status = intGraph.impl_addVertex(1);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyStorageShardTest, AddVertexString) {
  EXPECT_TRUE(stringGraph.impl_addVertex("D").isOK());

  auto status = stringGraph.impl_addVertex("A");
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyStorageShardTest, AddVertices) {
  std::vector<int> newVertices = {6, 7, 8, 9, 10};

  auto status = intGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  for (int vertex : newVertices) {
    auto neighbors = intGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
    EXPECT_TRUE(neighbors.first.empty());
  }
}

TEST_F(AdjacencyStorageShardTest, AddVerticesDuplicates) {
  std::vector<int> verticesWithDups = {6, 1, 7, 2, 8};

  auto status = intGraph.impl_addVertices(verticesWithDups);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_ALREADY_EXISTS);

  EXPECT_TRUE(intGraph.impl_getNeighbors(6).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(7).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(8).second.isOK());
}

TEST_F(AdjacencyStorageShardTest, AddVerticesEmpty) {
  std::vector<int> emptyVertices = {};

  auto status = intGraph.impl_addVertices(emptyVertices);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyStorageShardTest, AddVerticesString) {
  std::vector<std::string> newVertices = {"D", "E", "F"};

  auto status = stringGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  for (const auto &vertex : newVertices) {
    auto neighbors = stringGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
  }
}

TEST(AdjacencyListCustomTest, CustomVertexType) {
  PolicyHandler policy;
  AdjacencyList<CustomVertex, float> customGraph{policy};

  CustomVertex v1{1, "Node1"};
  CustomVertex v2{2, "Node2"};

  EXPECT_TRUE(customGraph.impl_addVertex(v1).isOK());
  EXPECT_TRUE(customGraph.impl_addVertex(v2).isOK());

  auto status = customGraph.impl_addVertex(v1);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Non Primitive Vertex Already Exists");

  EXPECT_TRUE(customGraph.impl_addEdge(v1, v2, 3.14f).isOK());

  auto edge = customGraph.impl_getEdge(v1, v2);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_FLOAT_EQ(edge.first, 3.14f);
}
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;
using namespace PeakStore;

// Base Fixture for Primitive Types
class AdjacencyStorageShardTest : public ::testing::Test, public CinderVertex {
protected:
  AdjacencyList<int, int> intGraph;
  AdjacencyList<std::string, float> stringGraph;

  void SetUp() override {
    intGraph.impl_addVertex(1);
    intGraph.impl_addVertex(2);
    intGraph.impl_addVertex(3);
    intGraph.impl_addVertex(4);
    intGraph.impl_addVertex(5);

    intGraph.impl_addVertex(101);
    intGraph.impl_addVertex(102);
    intGraph.impl_addVertex(103);

    stringGraph.impl_addVertex("A");
    stringGraph.impl_addVertex("B");
    stringGraph.impl_addVertex("C");
  }
};
// Base Fixture for Complex Types
class ComplexAdjVertex : public CinderVertex {
public:
  int vertexData;
  std::string nodeName;
  ComplexAdjVertex(int vertexData, std::string node_name)
      : vertexData{vertexData}, nodeName{node_name} {}
  ComplexAdjVertex() = default;
};

class ComplexAdjEdge : public CinderEdge {
public:
  float edgeValue;
  ComplexAdjEdge(float edgeValue) : edgeValue{edgeValue} {}
  ComplexAdjEdge() = default;
};

class ComplexGraph : public ::testing::Test {
public:
  ComplexAdjVertex v1, v2, v3;
  ComplexAdjEdge e1, e2;
  AdjacencyList<ComplexAdjVertex, ComplexAdjEdge> complexGraph;
  ComplexGraph() {
    v1 = ComplexAdjVertex(1, "Vertex1");
    v2 = ComplexAdjVertex(2, "Vertex2");
    v3 = ComplexAdjVertex(3, "Vertex3");

    e1 = ComplexAdjEdge(12.34);
    e2 = ComplexAdjEdge(76.45);

    complexGraph.impl_addVertex(v1);
    complexGraph.impl_addVertex(v2);
    complexGraph.impl_addVertex(v3);
  }
};
//
// 1. Vertex Operations
//

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

// Test to validate impl_hasVertex functionality
TEST_F(AdjacencyStorageShardTest, CheckVertexExistence) {
  EXPECT_TRUE(intGraph.impl_hasVertex(2));
  EXPECT_TRUE(intGraph.impl_hasVertex(5));
  EXPECT_FALSE(intGraph.impl_hasVertex(200));
}

//
// 2. Edge Operations
//

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

// Added test to validate removeEdge functionality
TEST_F(AdjacencyStorageShardTest, RemoveEdge) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());

  EXPECT_TRUE(intGraph.impl_removeEdge(1, 2).isOK());
  edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_FALSE(edge1.second.isOK());

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());

  EXPECT_TRUE(intGraph.impl_removeEdge(2, 3).isOK());
  edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_FALSE(edge2.second.isOK());

  EXPECT_FALSE(intGraph.impl_removeEdge(5, 6).isOK());
}

TEST_F(AdjacencyStorageShardTest, UpdateEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(101, 102, 7).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(103, 102, 5).isOK());

  EXPECT_TRUE(intGraph.impl_updateEdge(101, 102, 10).isOK());
  EXPECT_TRUE(intGraph.impl_updateEdge(103, 102, 1).isOK());
  EXPECT_FALSE(intGraph.impl_updateEdge(400, 102, 1).isOK());

  auto edge1 = intGraph.impl_getEdge(101, 102);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 10);

  auto edge2 = intGraph.impl_getEdge(103, 102);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 1);
}
TEST_F(ComplexGraph, UpdateEdgeOnComplexGraph) {
  ComplexAdjEdge newEdgeValue1(4.3);
  ComplexAdjEdge newEdgeValue2(467.32);
  EXPECT_TRUE(complexGraph.impl_addEdge(v1, v2, e1).isOK());
  EXPECT_TRUE(complexGraph.impl_addEdge(v2, v3, e2).isOK());

  complexGraph.impl_updateEdge(v1, v2, newEdgeValue1);
  EXPECT_EQ(complexGraph.impl_getEdge(v1, v2).first, newEdgeValue1);

  complexGraph.impl_updateEdge(v2, v3, newEdgeValue2);
  EXPECT_EQ(complexGraph.impl_getEdge(v2, v3).first, newEdgeValue2);
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

//
// 3. Edge Retrieval
//

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

//
// 4. Neighbor Retrieval
//

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

//
// 5. Edge Existence Checks
//

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

//
// 6. Adjacency List Structure
//

TEST_F(AdjacencyStorageShardTest, AdjacencyListStructure) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(1, 3, 10);
  intGraph.impl_addEdge(2, 3, 15);
  intGraph.impl_addEdge(4, 4, 4);
  intGraph.impl_addEdge(2, 5, 1);

  auto adjList = intGraph.getAdjList();

  EXPECT_EQ(adjList.size(), 8);

  auto it1 = adjList.find(1);
  ASSERT_NE(it1, adjList.end());
  EXPECT_EQ(it1->second.size(), 2);

  auto it2 = adjList.find(2);
  ASSERT_NE(it2, adjList.end());
  EXPECT_EQ(it2->second.size(), 2);

  auto it3 = adjList.find(3);
  ASSERT_NE(it3, adjList.end());
  EXPECT_TRUE(it3->second.empty());

  auto it4 = adjList.find(4);
  ASSERT_NE(it4, adjList.end());
  EXPECT_EQ(it4->second.size(), 1);

  auto it5 = adjList.find(5);
  ASSERT_NE(it5, adjList.end());
  EXPECT_TRUE(it5->second.empty());
}

//
// 7. Complex Type Tests (CustomVertex)
//
struct CustomVertex : public CinderVertex {
  int vertex_value;
  std::string name;
  CustomVertex(int v, std::string n) : vertex_value(v), name(n) {}
};

TEST(AdjacencyListCustomTest, CustomVertexType) {
  AdjacencyList<CustomVertex, float> customGraph;

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

//
// 8. Vertex Removal Tests (FEATURE: impl_removeVertex)
//

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
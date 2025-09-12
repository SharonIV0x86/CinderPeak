#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;
using namespace PeakStore;

// Base Fixture for Primitive Types
class AdjacencyListTest : public ::testing::Test, public CinderVertex {
protected:
  AdjacencyList<int, int> intGraph;
  AdjacencyList<std::string, float> stringGraph;

  void SetUp() override {
    intGraph.impl_addVertex(1);
    intGraph.impl_addVertex(2);
    intGraph.impl_addVertex(3);
    intGraph.impl_addVertex(4);
    intGraph.impl_addVertex(5);

    // New vertices added to validate updateEdge functionality
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

TEST_F(AdjacencyListTest, AddVertexPrimitive) {
  EXPECT_TRUE(intGraph.impl_addVertex(6).isOK());

  auto status = intGraph.impl_addVertex(1);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyListTest, AddVertexString) {
  EXPECT_TRUE(stringGraph.impl_addVertex("D").isOK());

  auto status = stringGraph.impl_addVertex("A");
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyListTest, AddVertices) {
  std::vector<int> newVertices = {6, 7, 8, 9, 10};

  auto status = intGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  // Verify all vertices were added
  for (int vertex : newVertices) {
    auto neighbors = intGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
    EXPECT_TRUE(
        neighbors.first.empty()); // New vertices should have no neighbors
  }
}

TEST_F(AdjacencyListTest, AddVerticesDuplicates) {
  std::vector<int> verticesWithDups = {6, 1, 7, 2, 8}; // 1 and 2 already exist

  auto status = intGraph.impl_addVertices(verticesWithDups);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_ALREADY_EXISTS);

  // Verify new vertices were still added
  EXPECT_TRUE(intGraph.impl_getNeighbors(6).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(7).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(8).second.isOK());
}

TEST_F(AdjacencyListTest, AddVerticesEmpty) {
  std::vector<int> emptyVertices = {};

  auto status = intGraph.impl_addVertices(emptyVertices);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyListTest, AddVerticesString) {
  std::vector<std::string> newVertices = {"D", "E", "F"};

  auto status = stringGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  // Verify vertices were added
  for (const auto &vertex : newVertices) {
    auto neighbors = stringGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
  }
}

//
// 2. Edge Operations
//

TEST_F(AdjacencyListTest, AddEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 5);

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 10);
}

// Added test to validate impl_updateEdge functionality
TEST_F(AdjacencyListTest, UpdateEdgeWithWeight) {
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

TEST_F(AdjacencyListTest, AddEdgeWithoutWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());

  auto edge = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_EQ(edge.first, 0); // Default int
}

TEST_F(AdjacencyListTest, AddEdgeInvalidVertices) {
  auto status1 = intGraph.impl_addEdge(99, 1);
  EXPECT_FALSE(status1.isOK());
  EXPECT_EQ(status1.code(), StatusCode::VERTEX_NOT_FOUND);

  auto status2 = intGraph.impl_addEdge(1, 99);
  EXPECT_FALSE(status2.isOK());
  EXPECT_EQ(status2.code(), StatusCode::VERTEX_NOT_FOUND);
}

TEST_F(AdjacencyListTest, AddEdgesPairs) {
  std::vector<std::pair<int, int>> edges = {
      {1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  // Verify all edges were added
  for (const auto &edge : edges) {
    auto result = intGraph.impl_getEdge(edge.first, edge.second);
    EXPECT_TRUE(result.second.isOK());
    EXPECT_EQ(result.first, 0); // Default weight
  }
}

TEST_F(AdjacencyListTest, AddEdgesTuples) {
  std::vector<std::tuple<int, int, int>> edges = {
      {1, 2, 10}, {2, 3, 20}, {3, 4, 30}, {4, 5, 40}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  // Verify all edges with correct weights
  EXPECT_EQ(intGraph.impl_getEdge(1, 2).first, 10);
  EXPECT_EQ(intGraph.impl_getEdge(2, 3).first, 20);
  EXPECT_EQ(intGraph.impl_getEdge(3, 4).first, 30);
  EXPECT_EQ(intGraph.impl_getEdge(4, 5).first, 40);
}

TEST_F(AdjacencyListTest, AddEdgesInvalidVertices) {
  std::vector<std::pair<int, int>> edgesWithInvalid = {
      {1, 2}, {99, 3}, {4, 5}, {1, 100} // 99 and 100 don't exist
  };

  auto status = intGraph.impl_addEdges(edgesWithInvalid);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_NOT_FOUND);

  // Verify valid edges were still added
  EXPECT_TRUE(intGraph.impl_getEdge(1, 2).second.isOK());
  EXPECT_TRUE(intGraph.impl_getEdge(4, 5).second.isOK());

  // Verify invalid edges were not added
  EXPECT_FALSE(intGraph.impl_getEdge(99, 3).second.isOK());
  EXPECT_FALSE(intGraph.impl_getEdge(1, 100).second.isOK());
}

TEST_F(AdjacencyListTest, AddEdgesEmpty) {
  std::vector<std::pair<int, int>> emptyEdges = {};

  auto status = intGraph.impl_addEdges(emptyEdges);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyListTest, AddEdgesMixedTypes) {
  // Test with string graph
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

TEST_F(AdjacencyListTest, GetExistingEdge) {
  intGraph.impl_addEdge(1, 2, 5);

  auto result = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(result.second.isOK());
  EXPECT_EQ(result.first, 5);
}

TEST_F(AdjacencyListTest, GetNonExistentEdge) {
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

TEST_F(AdjacencyListTest, GetNeighbors) {
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

TEST_F(AdjacencyListTest, GetNeighborsNonExistentVertex) {
  auto neighbors = intGraph.impl_getNeighbors(99);
  EXPECT_FALSE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);
  EXPECT_TRUE(neighbors.first.empty());
}

//
// 5. Edge Existence Checks
//

TEST_F(AdjacencyListTest, EdgeExistence) {
  intGraph.impl_addEdge(1, 2, 5);

  // Test edge existence through public interface instead of impl_doesEdgeExist
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

TEST_F(AdjacencyListTest, AdjacencyListStructure) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(1, 3, 10);
  intGraph.impl_addEdge(2, 3, 15);
  intGraph.impl_addEdge(4, 4, 4);
  intGraph.impl_addEdge(2, 5, 1);

  auto adjList = intGraph.getAdjList();

  EXPECT_EQ(adjList.size(), 8); // Updated value due to newer vertex additions

  auto it1 = adjList.find(1);
  ASSERT_NE(it1, adjList.end());
  EXPECT_EQ(it1->second.size(), 2);

  auto it2 = adjList.find(2);
  ASSERT_NE(it2, adjList.end());
  EXPECT_EQ(it2->second.size(), 2);

  auto it3 = adjList.find(3);
  ASSERT_NE(it3, adjList.end());
  EXPECT_TRUE(it3->second.empty());

  // Test for vertex 4
  auto it4 = adjList.find(4);
  ASSERT_NE(it4, adjList.end());
  EXPECT_EQ(it4->second.size(), 1); // vertex 4 has a self-loop

  // Test for vertex 5
  auto it5 = adjList.find(5);
  ASSERT_NE(it5, adjList.end());
  EXPECT_TRUE(it5->second.empty()); // vertex 5 has no outgoing edges
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
  // CustomVertex v1_dup{1, "Node1Duplicate"};

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
}

#include <gtest/gtest.h>
#include "StorageEngine/AdjacencyList.hpp"

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

        stringGraph.impl_addVertex("A");
        stringGraph.impl_addVertex("B");
    }
};

//
// 1. Vertex Operations
//

TEST_F(AdjacencyListTest, AddVertexPrimitive) {
    EXPECT_TRUE(intGraph.impl_addVertex(4).isOK());

    auto status = intGraph.impl_addVertex(1);
    EXPECT_FALSE(status.isOK());
    EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyListTest, AddVertexString) {
    EXPECT_TRUE(stringGraph.impl_addVertex("C").isOK());

    auto status = stringGraph.impl_addVertex("A");
    EXPECT_FALSE(status.isOK());
    EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
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

TEST_F(AdjacencyListTest, AddEdgeWithoutWeight) {
    EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());

    auto edge = intGraph.impl_getEdge(1, 2);
    EXPECT_TRUE(edge.second.isOK());
    EXPECT_EQ(edge.first, 0);  // Default int
}

TEST_F(AdjacencyListTest, AddEdgeInvalidVertices) {
    auto status1 = intGraph.impl_addEdge(99, 1);
    EXPECT_FALSE(status1.isOK());
    EXPECT_EQ(status1.code(), StatusCode::VERTEX_NOT_FOUND);

    auto status2 = intGraph.impl_addEdge(1, 99);
    EXPECT_FALSE(status2.isOK());
    EXPECT_EQ(status2.code(), StatusCode::VERTEX_NOT_FOUND);
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

    EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 2));
    EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 2, 5));

    EXPECT_FALSE(intGraph.impl_doesEdgeExist(1, 3));
    EXPECT_FALSE(intGraph.impl_doesEdgeExist(2, 1));
    EXPECT_FALSE(intGraph.impl_doesEdgeExist(99, 1));
}

//
// 6. Adjacency List Structure
//

TEST_F(AdjacencyListTest, AdjacencyListStructure) {
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(1, 3, 10);
    intGraph.impl_addEdge(2, 3, 15);

    auto adjList = intGraph.getAdjList();

    EXPECT_EQ(adjList.size(), 3);

    auto it1 = adjList.find(1);
    ASSERT_NE(it1, adjList.end());
    EXPECT_EQ(it1->second.size(), 2);

    auto it2 = adjList.find(2);
    ASSERT_NE(it2, adjList.end());
    EXPECT_EQ(it2->second.size(), 1);

    auto it3 = adjList.find(3);
    ASSERT_NE(it3, adjList.end());
    EXPECT_TRUE(it3->second.empty());
}

//
// 7. Complex Type Tests (CustomVertex)
//

struct CustomVertex : public CinderVertex {
    int vertex_value;
    std::string name;
    CustomVertex(int v, std::string n): vertex_value(v), name(n){}
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

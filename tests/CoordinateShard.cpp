#include <gtest/gtest.h>
#include "StorageEngine/CoordinateList.hpp"

using namespace CinderPeak;
using namespace PeakStore;

// Base Fixture for Primitive Types
class CoordinateListTest : public ::testing::Test {
protected:
    CoordinateList<int, int> intGraph;
    CoordinateList<std::string, float> stringGraph;

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

TEST_F(CoordinateListTest, AddVertexPrimitive) {
    EXPECT_TRUE(intGraph.impl_addVertex(4).isOK());

    auto status = intGraph.impl_addVertex(1);
    EXPECT_FALSE(status.isOK());
    EXPECT_EQ(status.message(), "Already Exists");
}

TEST_F(CoordinateListTest, AddVertexString) {
    EXPECT_TRUE(stringGraph.impl_addVertex("C").isOK());

    auto status = stringGraph.impl_addVertex("A");
    EXPECT_FALSE(status.isOK());
    EXPECT_EQ(status.message(), "Already Exists");
}

TEST_F(CoordinateListTest, HasVertex) {
    auto result = intGraph.impl_hasVertex(1);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_TRUE(result.second);

    result = intGraph.impl_hasVertex(99);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_FALSE(result.second);
}

TEST_F(CoordinateListTest, RemoveVertex) {
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(2, 3, 10);
    
    EXPECT_TRUE(intGraph.impl_removeVertex(2).isOK());
    
    auto edges = intGraph.impl_getAllEdges();
    EXPECT_TRUE(edges.first.isOK());
    EXPECT_TRUE(edges.second.empty());
    
    auto vertexResult = intGraph.impl_hasVertex(2);
    EXPECT_FALSE(vertexResult.second);
}

//
// 2. Edge Operations
//

TEST_F(CoordinateListTest, AddEdgeWithWeight) {
    EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
    EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

    auto edge1 = intGraph.impl_getEdge(1, 2);
    EXPECT_TRUE(edge1.second.isOK());
    EXPECT_EQ(edge1.first, 5);

    auto edge2 = intGraph.impl_getEdge(2, 3);
    EXPECT_TRUE(edge2.second.isOK());
    EXPECT_EQ(edge2.first, 10);
}

TEST_F(CoordinateListTest, AddEdgeWithoutWeight) {
    EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());

    auto edge = intGraph.impl_getEdge(1, 2);
    EXPECT_TRUE(edge.second.isOK());
    EXPECT_EQ(edge.first, 0);  // Default int
}

TEST_F(CoordinateListTest, HasEdge) {
    intGraph.impl_addEdge(1, 2, 5);
    
    auto result = intGraph.impl_hasEdge(1, 2);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_TRUE(result.second);
    
    result = intGraph.impl_hasEdge(2, 1);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_FALSE(result.second);
}

TEST_F(CoordinateListTest, RemoveEdge) {
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(2, 3, 10);
    
    EXPECT_TRUE(intGraph.impl_removeEdge(1, 2).isOK());
    
    auto edges = intGraph.impl_getAllEdges();
    EXPECT_TRUE(edges.first.isOK());
    EXPECT_EQ(edges.second.size(), 1);
    
    auto edgeResult = intGraph.impl_hasEdge(1, 2);
    EXPECT_FALSE(edgeResult.second);
}

TEST_F(CoordinateListTest, GetNonexistentEdge) {
    auto edge = intGraph.impl_getEdge(1, 99);
    EXPECT_FALSE(edge.second.isOK());
    EXPECT_EQ(edge.second.message(), "Edge not found");
}

//
// 3. Graph Queries
//

TEST_F(CoordinateListTest, GetAllVertices) {
    auto result = intGraph.impl_getAllVertices();
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second.size(), 3);
    
    std::unordered_set<int> expected = {1, 2, 3};
    for (auto v : result.second) {
        EXPECT_TRUE(expected.count(v) > 0);
    }
}

TEST_F(CoordinateListTest, GetAllEdges) {
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(2, 3, 10);
    
    auto result = intGraph.impl_getAllEdges();
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second.size(), 2);
    
    std::vector<std::tuple<int, int, int>> expected = {
        {1, 2, 5}, {2, 3, 10}
    };
    
    for (const auto& edge : result.second) {
        bool found = false;
        for (const auto& expected_edge : expected) {
            if (std::get<0>(edge) == std::get<0>(expected_edge) &&
                std::get<1>(edge) == std::get<1>(expected_edge) &&
                std::get<2>(edge) == std::get<2>(expected_edge)) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
}

TEST_F(CoordinateListTest, GetOutEdges) {
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(1, 3, 10);
    intGraph.impl_addEdge(2, 3, 15);
    
    auto result = intGraph.impl_getOutEdges(1);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second.size(), 2);
    
    std::vector<std::pair<int, int>> expected = {
        {2, 5}, {3, 10}
    };
    
    for (const auto& edge : result.second) {
        bool found = false;
        for (const auto& expected_edge : expected) {
            if (edge.first == expected_edge.first && 
                edge.second == expected_edge.second) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
}

TEST_F(CoordinateListTest, GetVertexCount) {
    auto result = intGraph.impl_getVertexCount();
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second, 3);
}

TEST_F(CoordinateListTest, GetEdgeCount) {
    intGraph.impl_addEdge(1, 2);
    intGraph.impl_addEdge(2, 3);
    
    auto result = intGraph.impl_getEdgeCount();
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second, 2);
}

TEST_F(CoordinateListTest, GetOutDegree) {
    intGraph.impl_addEdge(1, 2);
    intGraph.impl_addEdge(1, 3);
    intGraph.impl_addEdge(2, 3);
    
    auto result = intGraph.impl_getOutDegree(1);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second, 2);
}

TEST_F(CoordinateListTest, GetInDegree) {
    intGraph.impl_addEdge(1, 2);
    intGraph.impl_addEdge(1, 3);
    intGraph.impl_addEdge(2, 3);
    
    auto result = intGraph.impl_getInDegree(3);
    EXPECT_TRUE(result.first.isOK());
    EXPECT_EQ(result.second, 2);
}

//
// 4. Edge Existence Checks
//

TEST_F(CoordinateListTest, DoesEdgeExist) {
    intGraph.impl_addEdge(1, 2, 5);
    
    // This would test the unimplemented function, so we'll skip for now
    // EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 2, 5));
    // EXPECT_FALSE(intGraph.impl_doesEdgeExist(1, 2, 10));
    // EXPECT_FALSE(intGraph.impl_doesEdgeExist(2, 1));
}
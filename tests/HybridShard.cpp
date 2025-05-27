#include <gtest/gtest.h>
#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak;
using namespace PeakStore;

class HybridCSR_COOTest : public ::testing::Test
{
protected:
    HybridCSR_COO<int, int> intGraph;
    HybridCSR_COO<std::string, float> stringGraph;

    void SetUp() override
    {
        intGraph.impl_addVertex(1);
        intGraph.impl_addVertex(2);
        intGraph.impl_addVertex(3);

        stringGraph.impl_addVertex("A");
        stringGraph.impl_addVertex("B");
        stringGraph.impl_addVertex("C");
    }
};

// Vertex Operations
TEST_F(HybridCSR_COOTest, AddVertex)
{
    EXPECT_TRUE(intGraph.impl_addVertex(4).isOK());
    EXPECT_FALSE(intGraph.impl_addVertex(1).isOK());

    EXPECT_TRUE(stringGraph.impl_addVertex("D").isOK());
    EXPECT_FALSE(stringGraph.impl_addVertex("A").isOK());
}

TEST_F(HybridCSR_COOTest, RemoveVertex)
{
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.impl_addEdge(2, 3, 10);
    intGraph.buildStructures();
    EXPECT_TRUE(intGraph.impl_removeVertex(2).isOK());
    EXPECT_FALSE(intGraph.impl_doesEdgeExist(1, 2));
}

// Edge Operations
TEST_F(HybridCSR_COOTest, AddEdgeWithWeight)
{
    EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
    EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());
    intGraph.buildStructures();

    auto edge1 = intGraph.impl_getEdge(1, 2);
    EXPECT_TRUE(edge1.second.isOK());
    EXPECT_EQ(edge1.first, 5);

    auto edge2 = intGraph.impl_getEdge(2, 3);
    EXPECT_TRUE(edge2.second.isOK());
    EXPECT_EQ(edge2.first, 10);
}

TEST_F(HybridCSR_COOTest, AddEdgeWithoutWeight)
{
    EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());
    intGraph.buildStructures();

    auto edge = intGraph.impl_getEdge(1, 2);
    EXPECT_TRUE(edge.second.isOK());
    EXPECT_EQ(edge.first, 0);
}

TEST_F(HybridCSR_COOTest, EdgeExistence)
{
    intGraph.impl_addEdge(1, 2, 5);
    intGraph.buildStructures();
    EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 2));
    EXPECT_FALSE(intGraph.impl_doesEdgeExist(2, 1));
}

// Edge Cases
TEST_F(HybridCSR_COOTest, EmptyGraph)
{
    HybridCSR_COO<int, int> emptyGraph;
    emptyGraph.buildStructures();
    auto iter = emptyGraph.getNeighbors(1);
    EXPECT_FALSE(iter.hasNext());
}

TEST_F(HybridCSR_COOTest, SelfLoop)
{
    intGraph.impl_addEdge(1, 1, 5);
    intGraph.buildStructures();
    EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 1));
    auto edge = intGraph.impl_getEdge(1, 1);
    EXPECT_TRUE(edge.second.isOK());
    EXPECT_EQ(edge.first, 5);
}


// Stress Test
TEST_F(HybridCSR_COOTest, LargeGraph)
{
    constexpr size_t num_vertices = 10000;
    for (size_t i = 4; i <= num_vertices; ++i)
    {
        intGraph.impl_addVertex(static_cast<int>(i));
    }
    for (size_t i = 1; i < num_vertices; ++i)
    {
        intGraph.impl_addEdge(static_cast<int>(i), static_cast<int>(i + 1), static_cast<int>(i));
    }
    intGraph.buildStructures();

    for (size_t i = 1; i < num_vertices; ++i)
    {
        auto edge = intGraph.impl_getEdge(static_cast<int>(i), static_cast<int>(i + 1));
        EXPECT_TRUE(edge.second.isOK());
        EXPECT_EQ(edge.first, static_cast<int>(i));
    }
}
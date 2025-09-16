#include "gtest/gtest.h"
#include "CinderPeak.hpp"

using namespace CinderPeak;

// ---------------- Custom Types ----------------
class ListVertex : public CinderVertex {
public:
    int data;
    ListVertex(int d = 0) : data(d) {}
    bool operator==(const ListVertex &other) const { return data == other.data; }
};

class ListEdge : public CinderEdge {
public:
    float edge_weight;
    ListEdge(float w = 0.0f) : edge_weight(w) {}
    bool operator==(const ListEdge &other) const { return edge_weight == other.edge_weight; }
};

// ---------------- Test Fixture ----------------
class GraphListTest : public ::testing::Test {
protected:
    GraphCreationOptions directedOpts;
    GraphCreationOptions undirectedOpts;

    GraphListTest()
        : directedOpts({GraphCreationOptions::Directed}),
          undirectedOpts({GraphCreationOptions::Undirected}) {}
};

// Test 1: Weighted int edges (add, update, clear)
TEST_F(GraphListTest, WeightedEdgesAddUpdateClear) {
    GraphList<int, int> graph(directedOpts);
    GraphList<int, int>::setConsoleLogging(false);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    auto [e1, added1] = graph.addEdge(1, 3, 5);
    EXPECT_TRUE(added1);

    auto [prevW, updated] = graph.updateEdge(1, 3, 10);
    EXPECT_TRUE(updated);
    EXPECT_EQ(prevW, 5);

    EXPECT_EQ(graph.numVertices(), 3);
    EXPECT_EQ(graph.numEdges(), 1);

    graph.clearEdges();
    EXPECT_EQ(graph.numEdges(), 0);
    EXPECT_EQ(graph.numVertices(), 3); // vertices remain after clear
}

// Test 2: Unweighted graph (edge insertion and vertex count)
TEST_F(GraphListTest, UnweightedGraphEdges) {
    GraphList<int, Unweighted> g(directedOpts);

    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);

    auto [edgeKey, inserted] = g.addEdge(1, 2);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(edgeKey.first, 1);
    EXPECT_EQ(edgeKey.second, 2);

    EXPECT_TRUE(g.hasVertex(2));
    EXPECT_FALSE(g.hasVertex(99));

    EXPECT_EQ(g.numVertices(), 3);
    EXPECT_EQ(g.numEdges(), 1);
}

// Test 3: Custom vertex & edge types
TEST_F(GraphListTest, CustomVertexAndEdge) {
    ListVertex v1(1), v2(2);
    ListEdge e1(0.5f), e2(0.8f);

    GraphList<ListVertex, ListEdge> g(undirectedOpts);
    g.addVertex(v1);
    g.addVertex(v2);

    auto [weKey, added] = g.addEdge(v1, v2, e1);
    EXPECT_TRUE(added);

    auto [maybeEdgeBefore, foundBefore] = g.getEdge(v1, v2);
    ASSERT_TRUE(foundBefore);
    ASSERT_TRUE(maybeEdgeBefore.has_value());
    EXPECT_FLOAT_EQ(maybeEdgeBefore->edge_weight, 0.5f);

    auto [prevE, updated] = g.updateEdge(v1, v2, e2);
    EXPECT_TRUE(updated);
    EXPECT_FLOAT_EQ(prevE.edge_weight, 0.5f);

    auto [maybeEdgeAfter, foundAfter] = g.getEdge(v1, v2);
    ASSERT_TRUE(foundAfter);
    ASSERT_TRUE(maybeEdgeAfter.has_value());
    EXPECT_FLOAT_EQ(maybeEdgeAfter->edge_weight, 0.8f);
}

#include "gtest/gtest.h"
#include "CinderPeak.hpp"

using namespace CinderPeak;
using namespace CinderPeak::PeakStore;

// Custom Vertex Type
class CustomVertex : public CinderVertex {
public:
    int data;
    CustomVertex(int d = 0) : data(d) {}
    bool operator==(const CustomVertex &other) const { return data == other.data; }
};

// Custom Edge Type
class CustomEdge : public CinderEdge {
public:
    int dd;
    CustomEdge(int val = 0) : dd(val) {}
};

// Fixture for GraphMatrix tests
class GraphMatrixTest : public ::testing::Test {
protected:
    GraphCreationOptions options;
    GraphMatrix<CustomVertex, CustomEdge> graph;

    GraphMatrixTest()
        : options({GraphCreationOptions::Undirected, GraphCreationOptions::SelfLoops}),
          graph(options) {
        GraphMatrix<CustomVertex, CustomEdge>::setConsoleLogging(false);
    }
};

// Test 1: Adding vertices and edges
TEST_F(GraphMatrixTest, AddVerticesAndEdges) {
    CustomVertex v1(1), v2(2);
    CustomEdge e(100);

    auto v1Res = graph.addVertex(v1);
    auto v2Res = graph.addVertex(v2);

    EXPECT_TRUE(v1Res.second);
    EXPECT_TRUE(v2Res.second);

    auto edgeRes = graph.addEdge(v1, v2, e);
    EXPECT_TRUE(edgeRes.second);

    auto getEdge = graph.getEdge(v1, v2);
    ASSERT_TRUE(getEdge.second);
    EXPECT_EQ(getEdge.first->dd, 100);
}

// Test 2: Self-loop edge
TEST_F(GraphMatrixTest, AddSelfLoop) {
    CustomVertex v1(42);
    CustomEdge e(555);

    graph.addVertex(v1);
    auto loopRes = graph.addEdge(v1, v1, e);

    EXPECT_TRUE(loopRes.second);

    auto getLoop = graph.getEdge(v1, v1);
    ASSERT_TRUE(getLoop.second);
    EXPECT_EQ(getLoop.first->dd, 555);
}

// Test 3: Primitive type graph
TEST(GraphMatrixPrimitiveTest, IntVerticesAndEdges) {
    GraphCreationOptions options({GraphCreationOptions::Undirected, GraphCreationOptions::SelfLoops});
    GraphMatrix<int, int> g(options);

    g.addVertex(1);
    g.addVertex(2);

    auto edgeRes = g.addEdge(1, 2, 10);
    EXPECT_TRUE(edgeRes.second);

    auto getEdge = g.getEdge(1, 2);
    ASSERT_TRUE(getEdge.second);
    EXPECT_EQ(*getEdge.first, 10);
}

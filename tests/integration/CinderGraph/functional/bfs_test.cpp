#include "DummyGraphBuilder.hpp"
#include "gtest/gtest.h"
#include <vector>

using namespace CinderPeak;

class CinderGraphBfsTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphBfsTest, DirectedGraphBFS) {
  auto graph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);
  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);
  EXPECT_TRUE(graph.addVertex(4).second);

  EXPECT_TRUE(graph.addEdge(1, 2).second);
  EXPECT_TRUE(graph.addEdge(1, 3).second);
  EXPECT_TRUE(graph.addEdge(2, 4).second);

  auto result = graph.bfs(1);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3, 4}));
}

TEST_F(CinderGraphBfsTest, UndirectedGraphBFS) {
  auto graph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::undirected);
  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);
  EXPECT_TRUE(graph.addVertex(4).second);

  EXPECT_TRUE(graph.addEdge(1, 2).second);
  EXPECT_TRUE(graph.addEdge(1, 3).second);
  EXPECT_TRUE(graph.addEdge(3, 4).second);

  auto result = graph.bfs(1);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3, 4}));
}

TEST_F(CinderGraphBfsTest, DisconnectedGraphBFS) {
  auto graph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);
  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);
  EXPECT_TRUE(graph.addVertex(4).second);
  EXPECT_TRUE(graph.addVertex(5).second);

  EXPECT_TRUE(graph.addEdge(1, 2).second);
  EXPECT_TRUE(graph.addEdge(2, 3).second);

  auto result = graph.bfs(1);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3}));
}

TEST_F(CinderGraphBfsTest, IsolatedVertexTraversal) {
  auto graph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);
  EXPECT_TRUE(graph.addVertex(42).second);

  auto result = graph.bfs(42);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({42}));
}

TEST_F(CinderGraphBfsTest, MissingSourceReturnsNotFound) {
  auto graph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  auto result = graph.bfs(100);
  EXPECT_FALSE(result.isOK());
  EXPECT_EQ(result._status.code(), StatusCode::VERTEX_NOT_FOUND);
}

TEST_F(CinderGraphBfsTest, CustomVertexTraversal) {
  auto graph = builder.CreateCustomUnweightedGraph(GraphOpts::directed);
  ListVertex v1(1);
  ListVertex v2(2);
  ListVertex v3(3);

  EXPECT_TRUE(graph.addVertex(v1).second);
  EXPECT_TRUE(graph.addVertex(v2).second);
  EXPECT_TRUE(graph.addVertex(v3).second);

  EXPECT_TRUE(graph.addEdge(v1, v2).second);
  EXPECT_TRUE(graph.addEdge(v2, v3).second);

  auto result = graph.bfs(v1);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<ListVertex>({v1, v2, v3}));
}

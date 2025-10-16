#include "StorageEngine/HybridCSR_COO.hpp"
#include <gtest/gtest.h>

using namespace CinderPeak::PeakStore;

class HybridStorageShardTest : public ::testing::Test {
protected:
  void SetUp() override {
    graph = std::make_unique<HybridCSR_COO<int, int>>();
    string_graph = std::make_unique<HybridCSR_COO<std::string, double>>();
  }

  void TearDown() override {
    graph.reset();
    string_graph.reset();
  }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
  std::unique_ptr<HybridCSR_COO<std::string, double>> string_graph;
};

TEST_F(HybridStorageShardTest, ClearVerticesPrimitive) {
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    EXPECT_TRUE(graph->impl_addVertex(v).isOK());
  }

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int v : vertices) {
    EXPECT_FALSE(graph->impl_hasVertex(v));
  }
}

TEST_F(HybridStorageShardTest, ClearVerticesString) {
  std::vector<std::string> vertices = {"A", "B", "C", "D", "E"};
  for (const std::string &v : vertices) {
    EXPECT_TRUE(string_graph->impl_addVertex(v).isOK());
  }

  EXPECT_TRUE(string_graph->impl_clearVertices().isOK());

  for (const std::string &v : vertices) {
    EXPECT_FALSE(string_graph->impl_hasVertex(v));
  }
}

TEST_F(HybridStorageShardTest, ClearVerticesWithEdges) {

  for (int i = 1; i <= 5; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());
  EXPECT_TRUE(graph->impl_addEdge(3, 4, 30).isOK());
  EXPECT_TRUE(graph->impl_addEdge(4, 5, 40).isOK());
  EXPECT_TRUE(graph->impl_addEdge(5, 1, 50).isOK());

  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2));
  EXPECT_TRUE(graph->impl_doesEdgeExist(3, 4));

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int i = 1; i <= 5; i++) {
    EXPECT_FALSE(graph->impl_hasVertex(i));
  }

  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2));
  EXPECT_FALSE(graph->impl_doesEdgeExist(3, 4));
}

TEST_F(HybridStorageShardTest, ClearVerticesWithCOOBuffer) {
  for (int i = 1; i <= 10; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  for (int i = 1; i < 10; i++) {
    EXPECT_TRUE(graph->impl_addEdge(i, i + 1, i * 10).isOK());
  }

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int i = 1; i <= 10; i++) {
    EXPECT_FALSE(graph->impl_hasVertex(i));
  }
}

TEST_F(HybridStorageShardTest, ClearVerticesAfterVertexRemoval) {
  for (int i = 1; i <= 5; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());

  EXPECT_TRUE(graph->impl_removeVertex(2).isOK());
  EXPECT_FALSE(graph->impl_hasVertex(2));

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int i = 1; i <= 5; i++) {
    EXPECT_FALSE(graph->impl_hasVertex(i));
  }
}

TEST_F(HybridStorageShardTest, ClearVerticesAndReuse) {

  for (int i = 1; i <= 5; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int i = 10; i <= 15; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  EXPECT_TRUE(graph->impl_addEdge(10, 11, 100).isOK());
  EXPECT_TRUE(graph->impl_addEdge(11, 12, 110).isOK());

  EXPECT_TRUE(graph->impl_hasVertex(10));
  EXPECT_TRUE(graph->impl_hasVertex(15));
  EXPECT_FALSE(graph->impl_hasVertex(1));
  EXPECT_FALSE(graph->impl_hasVertex(5));

  EXPECT_TRUE(graph->impl_doesEdgeExist(10, 11));
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2));

  auto edge = graph->impl_getEdge(10, 11);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_EQ(edge.first, 100);
}

TEST_F(HybridStorageShardTest, ClearVerticesLargeGraph) {
  const int num_vertices = 100000;
  for (int i = 0; i < num_vertices; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  for (int i = 0; i < num_vertices - 1; i++) {
    EXPECT_TRUE(graph->impl_addEdge(i, i + 1, i).isOK());
  }

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  EXPECT_FALSE(graph->impl_hasVertex(0));
  EXPECT_FALSE(graph->impl_hasVertex(500));
  EXPECT_FALSE(graph->impl_hasVertex(999));
}

TEST_F(HybridStorageShardTest, ClearVerticesWithSelfLoops) {
  for (int i = 1; i <= 5; i++) {
    EXPECT_TRUE(graph->impl_addVertex(i).isOK());
  }

  for (int i = 1; i <= 5; i++) {
    EXPECT_TRUE(graph->impl_addEdge(i, i, i * 100).isOK());
  }

  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int i = 1; i <= 5; i++) {
    EXPECT_FALSE(graph->impl_hasVertex(i));
  }
}
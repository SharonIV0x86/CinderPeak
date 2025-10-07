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

// Test clearing edges in a graph
TEST_F(HybridStorageShardTest, ClearEdges_Basic) {
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    graph->impl_addVertex(v);
  }
  EXPECT_TRUE(graph->impl_addEdge(1, 2).isOK()) << "Failed to add edge (1,2)";
  EXPECT_TRUE(graph->impl_addEdge(2, 3).isOK()) << "Failed to add edge (2,3)";
  EXPECT_TRUE(graph->impl_addEdge(1, 3).isOK()) << "Failed to add edge (1,3)";
  EXPECT_TRUE(graph->impl_addEdge(4, 5).isOK()) << "Failed to add edge (4,5)";
  EXPECT_TRUE(graph->impl_addEdge(1, 5).isOK()) << "Failed to add edge (1,5)";
  EXPECT_TRUE(graph->impl_getEdge(1, 2).second.isOK())
      << "Edge (1,2) not found";
  EXPECT_TRUE(graph->impl_getEdge(4, 5).second.isOK())
      << "Edge (4,5) not found";
  EXPECT_TRUE(graph->impl_clearEdges().isOK()) << "Failed to clear edges";
  EXPECT_FALSE(graph->impl_getEdge(1, 2).second.isOK())
      << "Edge (1,2) still exists";
  EXPECT_FALSE(graph->impl_getEdge(4, 5).second.isOK())
      << "Edge (4,5) still exists";
  for (int v : vertices) {
    EXPECT_TRUE(graph->impl_hasVertex(v))
        << "Vertex " << v << " removed unexpectedly";
  }
}

// Test clearing edges in an empty graph
TEST_F(HybridStorageShardTest, ClearEdges_EmptyGraph) {
  EXPECT_TRUE(graph->impl_clearEdges().isOK())
      << "Failed to clear edges in empty graph";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2)) << "Edge exists in empty graph";
}

// Test clearing edges in string graph
TEST_F(HybridStorageShardTest, ClearEdges_String) {
  string_graph->impl_addVertex("prasad");
  string_graph->impl_addVertex("omkar");
  string_graph->impl_addEdge("prasad", "omkar", 1.5);
  EXPECT_TRUE(string_graph->impl_clearEdges().isOK())
      << "Failed to clear string edges";
  EXPECT_FALSE(string_graph->impl_doesEdgeExist("prasad", "omkar"))
      << "String edge still exists";
  EXPECT_TRUE(string_graph->impl_hasVertex("prasad"))
      << "String vertex removed unexpectedly";
  EXPECT_TRUE(string_graph->impl_hasVertex("omkar"))
      << "String vertex removed unexpectedly";
}
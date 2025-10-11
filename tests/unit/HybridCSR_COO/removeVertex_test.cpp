#include "StorageEngine/HybridCSR_COO.hpp"
#include <gtest/gtest.h>
#include <thread>

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

// Test removing a valid vertex with edges
TEST_F(HybridStorageShardTest, RemoveVertex_Valid) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 10);
  auto status = graph->impl_removeVertex(1);
  EXPECT_TRUE(status.isOK()) << "Failed to remove vertex 1";
  EXPECT_FALSE(graph->impl_hasVertex(1)) << "Vertex 1 still exists";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2)) << "Edge (1,2) still exists";
  EXPECT_TRUE(graph->impl_hasVertex(2)) << "Vertex 2 removed unexpectedly";
}

// Test removing a non-existent vertex
TEST_F(HybridStorageShardTest, RemoveVertex_NonExistent) {
  auto status = graph->impl_removeVertex(999);
  EXPECT_FALSE(status.isOK()) << "Removed non-existent vertex 999";
}

// Test removing a vertex with multiple edges
TEST_F(HybridStorageShardTest, RemoveVertex_WithMultipleEdges) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addVertex(3);
  graph->impl_addEdge(1, 2, 10);
  graph->impl_addEdge(1, 3, 20);
  graph->impl_addEdge(2, 1, 15);
  auto status = graph->impl_removeVertex(1);
  EXPECT_TRUE(status.isOK()) << "Failed to remove vertex 1";
  EXPECT_FALSE(graph->impl_hasVertex(1)) << "Vertex 1 still exists";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2)) << "Edge (1,2) still exists";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 3)) << "Edge (1,3) still exists";
  EXPECT_FALSE(graph->impl_doesEdgeExist(2, 1)) << "Edge (2,1) still exists";
}

// Test removing a string vertex
TEST_F(HybridStorageShardTest, RemoveVertex_String) {
  string_graph->impl_addVertex("prasad");
  string_graph->impl_addVertex("omkar");
  string_graph->impl_addEdge("prasad", "omkar", 1.5);
  auto status = string_graph->impl_removeVertex("prasad");
  EXPECT_TRUE(status.isOK()) << "Failed to remove string vertex";
  EXPECT_FALSE(string_graph->impl_hasVertex("prasad"))
      << "String vertex still exists";
  EXPECT_FALSE(string_graph->impl_doesEdgeExist("prasad", "omkar"))
      << "Edge still exists";
}

// Test concurrent vertex removal
TEST_F(HybridStorageShardTest, RemoveVertex_Concurrent) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  std::vector<std::thread> threads;
  threads.emplace_back([this]() {
    auto status = graph->impl_removeVertex(1);
    EXPECT_TRUE(status.isOK()) << "Thread failed to remove vertex 1";
  });
  threads.emplace_back([this]() {
    auto status = graph->impl_removeVertex(2);
    EXPECT_TRUE(status.isOK()) << "Thread failed to remove vertex 2";
  });
  for (auto &t : threads) {
    t.join();
  }
  EXPECT_FALSE(graph->impl_hasVertex(1)) << "Concurrent vertex 1 still exists";
  EXPECT_FALSE(graph->impl_hasVertex(2)) << "Concurrent vertex 2 still exists";
}
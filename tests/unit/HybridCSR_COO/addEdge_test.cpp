#include "StorageEngine/HybridCSR_COO.hpp"
#include <gtest/gtest.h>
#include <random>
#include <set>
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

  std::vector<std::tuple<int, int, int>>
  generateTestEdges(int num_vertices, int num_edges, int seed = 42) {
    std::vector<std::tuple<int, int, int>> edges;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> vertex_dis(0, num_vertices - 1);
    std::set<std::pair<int, int>> edge_set;
    while (edges.size() < static_cast<size_t>(num_edges)) {
      int src = vertex_dis(gen);
      int dest = vertex_dis(gen);
      if (src != dest && edge_set.find({src, dest}) == edge_set.end()) {
        edge_set.insert({src, dest});
        edges.emplace_back(src, dest, src * 1000 + dest);
      }
    }
    return edges;
  }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
  std::unique_ptr<HybridCSR_COO<std::string, double>> string_graph;
};

// Test basic edge addition
TEST_F(HybridStorageShardTest, AddEdge_Basic) {
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    graph->impl_addVertex(v);
  }
  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK())
      << "Failed to add edge (1,2)";
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK())
      << "Failed to add edge (2,3)";
  EXPECT_TRUE(graph->impl_addEdge(1, 3, 15).isOK())
      << "Failed to add edge (1,3)";
  EXPECT_TRUE(graph->impl_addEdge(4, 5, 25).isOK())
      << "Failed to add edge (4,5)";
  EXPECT_TRUE(graph->impl_addEdge(1, 5, 35).isOK())
      << "Failed to add edge (1,5)";
  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(w1, 10) << "Incorrect weight for edge (1,2)";
  auto [w2, s2] = graph->impl_getEdge(4, 5);
  EXPECT_TRUE(s2.isOK()) << "Edge (4,5) not found";
  EXPECT_EQ(w2, 25) << "Incorrect weight for edge (4,5)";
}

// Test adding edges with non-existent vertices
TEST_F(HybridStorageShardTest, AddEdge_NonExistentVertices) {
  graph->impl_addVertex(1);
  EXPECT_FALSE(graph->impl_addEdge(99, 1, 10).isOK())
      << "Added edge with non-existent source";
  EXPECT_FALSE(graph->impl_addEdge(1, 99, 10).isOK())
      << "Added edge with non-existent destination";
  EXPECT_FALSE(graph->impl_addEdge(88, 99, 10).isOK())
      << "Added edge with both vertices non-existent";
}

// Test adding edges with negative weights
TEST_F(HybridStorageShardTest, AddEdge_NegativeWeights) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  EXPECT_TRUE(graph->impl_addEdge(1, 2, -100).isOK())
      << "Failed to add edge with negative weight";
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(weight, -100) << "Incorrect negative weight";
}

// Test adding edges with zero weights
TEST_F(HybridStorageShardTest, AddEdge_ZeroWeights) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  EXPECT_TRUE(graph->impl_addEdge(1, 2, 0).isOK())
      << "Failed to add edge with zero weight";
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(weight, 0) << "Incorrect zero weight";
}

// Test COO buffer priority (multiple edges)
TEST_F(HybridStorageShardTest, AddEdge_COOBufferPriority) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 100);
  for (int i = 3; i < 1027; ++i) {
    graph->impl_addVertex(i);
    graph->impl_addEdge(1, i, i);
  }
  graph->impl_addEdge(1, 2, 999);
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(weight, 999) << "Incorrect weight after COO buffer priority";
}

// Test COO buffer overwrite
TEST_F(HybridStorageShardTest, AddEdge_COOBufferOverwrite) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 10);
  graph->impl_addEdge(1, 2, 20);
  graph->impl_addEdge(1, 2, 30);
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(weight, 30) << "Incorrect weight after overwrite";
}

// Test adding edges with string vertices
TEST_F(HybridStorageShardTest, AddEdge_String) {
  string_graph->impl_addVertex("prasad");
  string_graph->impl_addVertex("omkar");
  string_graph->impl_addVertex("tejas");
  EXPECT_TRUE(string_graph->impl_addEdge("prasad", "omkar", 1.5).isOK())
      << "Failed to add string edge";
  EXPECT_TRUE(string_graph->impl_addEdge("omkar", "tejas", 2.7).isOK())
      << "Failed to add string edge";
  EXPECT_TRUE(string_graph->impl_addEdge("prasad", "tejas", 3.14159).isOK())
      << "Failed to add string edge";
  auto [w1, s1] = string_graph->impl_getEdge("prasad", "omkar");
  EXPECT_TRUE(s1.isOK()) << "String edge (prasad,omkar) not found";
  EXPECT_DOUBLE_EQ(w1, 1.5) << "Incorrect weight for string edge";
}

// Test adding self-loop edge
TEST_F(HybridStorageShardTest, AddEdge_SelfLoop) {
  graph->impl_addVertex(1);
  EXPECT_TRUE(graph->impl_addEdge(1, 1, 10).isOK())
      << "Failed to add self-loop";
  auto [weight, status] = graph->impl_getEdge(1, 1);
  EXPECT_TRUE(status.isOK()) << "Self-loop edge not found";
  EXPECT_EQ(weight, 10) << "Incorrect self-loop weight";
}

// Test concurrent edge addition
TEST_F(HybridStorageShardTest, AddEdge_Concurrent) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  std::vector<std::thread> threads;
  const int NUM_THREADS = 5;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this, i]() {
      auto status = graph->impl_addEdge(1, 2, 10 + i);
      EXPECT_TRUE(status.isOK()) << "Thread " << i << " failed to add edge";
    });
  }
  for (auto &t : threads) {
    t.join();
  }
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Concurrent edge not found";
}
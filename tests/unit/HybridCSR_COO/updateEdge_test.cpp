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

// Test updating edge weights
TEST_F(HybridStorageShardTest, UpdateEdge_Basic) {
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
  EXPECT_TRUE(graph->impl_updateEdge(1, 2, 15).isOK())
      << "Failed to update edge (1,2)";
  EXPECT_TRUE(graph->impl_updateEdge(2, 3, 10).isOK())
      << "Failed to update edge (2,3)";
  EXPECT_FALSE(graph->impl_updateEdge(547, 3, 15).isOK())
      << "Updated non-existent edge";
  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(w1, 15) << "Incorrect updated weight for (1,2)";
  auto [w2, s2] = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(s2.isOK()) << "Edge (2,3) not found";
  EXPECT_EQ(w2, 10) << "Incorrect updated weight for (2,3)";
}

// Test updating non-existent edge
TEST_F(HybridStorageShardTest, UpdateEdge_NonExistent) {
  graph->impl_addVertex(1);
  EXPECT_FALSE(graph->impl_updateEdge(1, 2, 10).isOK())
      << "Updated non-existent edge";
}

// Test updating edge in string graph
TEST_F(HybridStorageShardTest, UpdateEdge_String) {
  string_graph->impl_addVertex("prasad");
  string_graph->impl_addVertex("omkar");
  string_graph->impl_addEdge("prasad", "omkar", 1.5);
  EXPECT_TRUE(string_graph->impl_updateEdge("prasad", "omkar", 2.0).isOK())
      << "Failed to update string edge";
  auto [weight, status] = string_graph->impl_getEdge("prasad", "omkar");
  EXPECT_TRUE(status.isOK()) << "String edge not found";
  EXPECT_DOUBLE_EQ(weight, 2.0) << "Incorrect updated weight for string edge";
}

// Test concurrent edge updates
TEST_F(HybridStorageShardTest, UpdateEdge_Concurrent) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 10);
  std::vector<std::thread> threads;
  const int NUM_THREADS = 5;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this, i]() {
      auto status = graph->impl_updateEdge(1, 2, 10 + i);
      EXPECT_TRUE(status.isOK()) << "Thread " << i << " failed to update edge";
    });
  }
  for (auto &t : threads) {
    t.join();
  }
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK()) << "Concurrent edge not found";
}
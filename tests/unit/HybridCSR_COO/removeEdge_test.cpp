#include "StorageEngine/HybridCSR_COO.hpp"
#include <atomic>
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

// Test removing edges with weights
TEST_F(HybridStorageShardTest, RemoveEdge_WithWeight) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addVertex(3);
  EXPECT_TRUE(graph->impl_addEdge(1, 2, 5).isOK())
      << "Failed to add edge (1,2)";
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 10).isOK())
      << "Failed to add edge (2,3)";
  auto edge1 = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK()) << "Edge (1,2) not found";
  auto result1 = graph->impl_removeEdge(1, 2);
  EXPECT_TRUE(result1.second.isOK()) << "Failed to remove edge (1,2)";
  EXPECT_EQ(result1.first, edge1.first) << "Incorrect weight returned";
  edge1 = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(edge1.second.isOK()) << "Edge (1,2) still exists";
  auto edge2 = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK()) << "Edge (2,3) not found";
  auto result2 = graph->impl_removeEdge(2, 3);
  EXPECT_TRUE(result2.second.isOK()) << "Failed to remove edge (2,3)";
  EXPECT_EQ(result2.first, edge2.first) << "Incorrect weight returned";
  edge2 = graph->impl_getEdge(2, 3);
  EXPECT_FALSE(edge2.second.isOK()) << "Edge (2,3) still exists";
  EXPECT_FALSE(graph->impl_removeEdge(5, 6).second.isOK())
      << "Removed non-existent edge";
}

// Test removing edges without weights
TEST_F(HybridStorageShardTest, RemoveEdge_WithoutWeight) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addVertex(3);
  EXPECT_TRUE(graph->impl_addEdge(1, 2).isOK()) << "Failed to add edge (1,2)";
  EXPECT_TRUE(graph->impl_addEdge(2, 3).isOK()) << "Failed to add edge (2,3)";
  auto edge1 = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK()) << "Edge (1,2) not found";
  EXPECT_TRUE(graph->impl_removeEdge(1, 2).second.isOK())
      << "Failed to remove edge (1,2)";
  edge1 = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(edge1.second.isOK()) << "Edge (1,2) still exists";
  auto edge2 = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK()) << "Edge (2,3) not found";
  EXPECT_TRUE(graph->impl_removeEdge(2, 3).second.isOK())
      << "Failed to remove edge (2,3)";
  edge2 = graph->impl_getEdge(2, 3);
  EXPECT_FALSE(edge2.second.isOK()) << "Edge (2,3) still exists";
  EXPECT_FALSE(graph->impl_removeEdge(5, 6).second.isOK())
      << "Removed non-existent edge";
}

// Test removing edges in string graph
TEST_F(HybridStorageShardTest, RemoveEdge_String) {
  string_graph->impl_addVertex("prasad");
  string_graph->impl_addVertex("omkar");
  string_graph->impl_addEdge("prasad", "omkar", 1.5);
  auto result = string_graph->impl_removeEdge("prasad", "omkar");
  EXPECT_TRUE(result.second.isOK()) << "Failed to remove string edge";
  EXPECT_DOUBLE_EQ(result.first, 1.5) << "Incorrect weight returned";
  EXPECT_FALSE(string_graph->impl_doesEdgeExist("prasad", "omkar"))
      << "String edge still exists";
}

// Test concurrent edge removal
TEST_F(HybridStorageShardTest, RemoveEdge_Concurrent) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 10);
  std::vector<std::thread> threads;
  const int NUM_THREADS = 5;
  std::atomic<int> success_count{0};
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this, &success_count]() {
      auto result = graph->impl_removeEdge(1, 2);
      if (result.second.isOK()) {
        EXPECT_EQ(result.first, 10)
            << "Removed edge (1,2) should return weight 10";
        success_count++;
      } else {
        EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2))
            << "Edge (1,2) should not exist after another thread removed it";
      }
    });
  }
  for (auto &t : threads) {
    t.join();
  }
  EXPECT_EQ(success_count, 1)
      << "Exactly one thread should successfully remove edge (1,2)";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2))
      << "Edge (1,2) should not exist after concurrent removal";
}
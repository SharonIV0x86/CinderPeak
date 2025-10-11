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

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
  std::unique_ptr<HybridCSR_COO<std::string, double>> string_graph;
};

// Test checking edges in empty graph
TEST_F(HybridStorageShardTest, DoesEdgeExist_EmptyGraph) {
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2)) << "Edge found in empty graph";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2, 10))
      << "Weighted edge found in empty graph";
}

// Test advanced edge existence checks
TEST_F(HybridStorageShardTest, DoesEdgeExist_Advanced) {
  for (int i = 1; i <= 5; ++i) {
    graph->impl_addVertex(i);
  }
  graph->impl_addEdge(1, 2, 12);
  graph->impl_addEdge(1, 3, 13);
  graph->impl_addEdge(1, 4, 14);
  graph->impl_addEdge(2, 3, 23);
  graph->impl_addEdge(3, 4, 34);
  graph->impl_addEdge(4, 5, 45);
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2)) << "Edge (1,2) not found";
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 3)) << "Edge (1,3) not found";
  EXPECT_TRUE(graph->impl_doesEdgeExist(4, 5)) << "Edge (4,5) not found";
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2, 12))
      << "Weighted edge (1,2,12) not found";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2, 99))
      << "Incorrect weighted edge found";
  EXPECT_FALSE(graph->impl_doesEdgeExist(2, 1))
      << "Non-existent edge (2,1) found";
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 5))
      << "Non-existent edge (1,5) found";
  EXPECT_FALSE(graph->impl_doesEdgeExist(5, 1))
      << "Non-existent edge (5,1) found";
}

// Test concurrent edge existence checks
TEST_F(HybridStorageShardTest, DoesEdgeExist_Concurrent) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 10);
  std::vector<std::thread> threads;
  const int NUM_THREADS = 10;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this]() {
      EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2))
          << "Concurrent edge check failed";
    });
  }
  for (auto &t : threads) {
    t.join();
  }
}
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

// Test checking vertex existence
TEST_F(HybridStorageShardTest, HasVertex_Basic) {
  EXPECT_TRUE(graph->impl_addVertex(40).isOK()) << "Failed to add vertex 40";
  EXPECT_TRUE(graph->impl_addVertex(49).isOK()) << "Failed to add vertex 49";
  EXPECT_TRUE(graph->impl_hasVertex(40)) << "Vertex 40 not found";
  EXPECT_TRUE(graph->impl_hasVertex(49)) << "Vertex 49 not found";
  EXPECT_FALSE(graph->impl_hasVertex(404)) << "Non-existent vertex 404 found";
}

// Test checking non-existent vertex
TEST_F(HybridStorageShardTest, HasVertex_NonExistent) {
  EXPECT_FALSE(graph->impl_hasVertex(999)) << "Non-existent vertex 999 found";
}

// Test checking vertex in string graph
TEST_F(HybridStorageShardTest, HasVertex_String) {
  string_graph->impl_addVertex("prasad");
  EXPECT_TRUE(string_graph->impl_hasVertex("prasad"))
      << "String vertex not found";
  EXPECT_FALSE(string_graph->impl_hasVertex("omkar"))
      << "Non-existent string vertex found";
}

// Test concurrent vertex checks
TEST_F(HybridStorageShardTest, HasVertex_Concurrent) {
  graph->impl_addVertex(1);
  std::vector<std::thread> threads;
  const int NUM_THREADS = 10;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this]() {
      EXPECT_TRUE(graph->impl_hasVertex(1)) << "Concurrent vertex check failed";
    });
  }
  for (auto &t : threads) {
    t.join();
  }
}
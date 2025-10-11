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

// Test adding a single vertex and checking for duplicates
TEST_F(HybridStorageShardTest, AddVertex_Single) {
  auto status = graph->impl_addVertex(42);
  EXPECT_TRUE(status.isOK()) << "Failed to add vertex 42";
  auto duplicate_status = graph->impl_addVertex(42);
  EXPECT_FALSE(duplicate_status.isOK()) << "Duplicate vertex 42 added";
  EXPECT_TRUE(graph->impl_hasVertex(42)) << "Vertex 42 not found";
}

// Test sequential vertex addition
TEST_F(HybridStorageShardTest, AddVertex_Sequential) {
  std::vector<int> vertices = {1, 5, 3, 9, 2, 7};
  for (int v : vertices) {
    auto status = graph->impl_addVertex(v);
    EXPECT_TRUE(status.isOK()) << "Failed to add vertex " << v;
  }
  for (int v : vertices) {
    auto status = graph->impl_addVertex(v);
    EXPECT_FALSE(status.isOK()) << "Duplicate vertex " << v << " added";
    EXPECT_TRUE(graph->impl_hasVertex(v)) << "Vertex " << v << " not found";
  }
}

// Test adding vertices with large IDs
TEST_F(HybridStorageShardTest, AddVertex_LargeIDs) {
  int large_id1 = 1000000;
  int large_id2 = 2000000;
  EXPECT_TRUE(graph->impl_addVertex(large_id1).isOK())
      << "Failed to add large vertex ID";
  EXPECT_TRUE(graph->impl_addVertex(large_id2).isOK())
      << "Failed to add large vertex ID";
  EXPECT_TRUE(graph->impl_hasVertex(large_id1)) << "Large vertex ID not found";
  EXPECT_TRUE(graph->impl_hasVertex(large_id2)) << "Large vertex ID not found";
}

// Test adding many vertices
TEST_F(HybridStorageShardTest, AddVertex_ManyVertices) {
  const int NUM_VERTICES = 10000;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    auto status = graph->impl_addVertex(i);
    EXPECT_TRUE(status.isOK()) << "Failed to add vertex " << i;
  }
  for (int i = 0; i < NUM_VERTICES; ++i) {
    EXPECT_TRUE(graph->impl_hasVertex(i)) << "Vertex " << i << " not found";
  }
}

// Test adding string vertices
TEST_F(HybridStorageShardTest, AddVertex_String) {
  EXPECT_TRUE(string_graph->impl_addVertex("prasad").isOK())
      << "Failed to add string vertex";
  EXPECT_TRUE(string_graph->impl_addVertex("omkar").isOK())
      << "Failed to add string vertex";
  EXPECT_TRUE(string_graph->impl_addVertex("tejas").isOK())
      << "Failed to add string vertex";
  EXPECT_TRUE(string_graph->impl_hasVertex("prasad"))
      << "String vertex not found";
  EXPECT_TRUE(string_graph->impl_hasVertex("omkar"))
      << "String vertex not found";
  EXPECT_TRUE(string_graph->impl_hasVertex("tejas"))
      << "String vertex not found";
}

// Test concurrent vertex addition (thread-safety)
TEST_F(HybridStorageShardTest, AddVertex_Concurrent) {
  std::vector<std::thread> threads;
  const int NUM_THREADS = 10;
  for (int i = 0; i < NUM_THREADS; ++i) {
    threads.emplace_back([this, i]() {
      auto status = graph->impl_addVertex(i);
      EXPECT_TRUE(status.isOK()) << "Thread " << i << " failed to add vertex";
    });
  }
  for (auto &t : threads) {
    t.join();
  }
  for (int i = 0; i < NUM_THREADS; ++i) {
    EXPECT_TRUE(graph->impl_hasVertex(i))
        << "Concurrent vertex " << i << " not found";
  }
}

// Test adding vertex with negative ID
TEST_F(HybridStorageShardTest, AddVertex_NegativeID) {
  auto status = graph->impl_addVertex(-1);
  EXPECT_TRUE(status.isOK()) << "Failed to add negative vertex ID";
  EXPECT_TRUE(graph->impl_hasVertex(-1)) << "Negative vertex ID not found";
}
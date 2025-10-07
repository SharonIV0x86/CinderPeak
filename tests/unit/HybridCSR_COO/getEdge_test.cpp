#include "StorageEngine/HybridCSR_COO.hpp"
#include <gtest/gtest.h>
#include <random>
#include <set>

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

// Test getting edge in empty graph
TEST_F(HybridStorageShardTest, GetEdge_EmptyGraph) {
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(status.isOK()) << "Edge found in empty graph";
}

// Test getting self-loop edge
TEST_F(HybridStorageShardTest, GetEdge_SelfLoop) {
  graph->impl_addVertex(42);
  graph->impl_addEdge(42, 42, 100);
  auto [weight, status] = graph->impl_getEdge(42, 42);
  EXPECT_TRUE(status.isOK()) << "Self-loop edge not found";
  EXPECT_EQ(weight, 100) << "Incorrect self-loop weight";
}

// Test advanced edge retrieval
TEST_F(HybridStorageShardTest, GetEdge_Advanced) {
  for (int i = 1; i <= 5; ++i) {
    graph->impl_addVertex(i);
  }
  graph->impl_addEdge(1, 2, 12);
  graph->impl_addEdge(1, 3, 13);
  graph->impl_addEdge(1, 4, 14);
  graph->impl_addEdge(2, 3, 23);
  graph->impl_addEdge(3, 4, 34);
  graph->impl_addEdge(4, 5, 45);
  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(w1, 12) << "Incorrect weight for edge (1,2)";
  auto [w2, s2] = graph->impl_getEdge(4, 5);
  EXPECT_TRUE(s2.isOK()) << "Edge (4,5) not found";
  EXPECT_EQ(w2, 45) << "Incorrect weight for edge (4,5)";
  auto [w3, s3] = graph->impl_getEdge(2, 1);
  EXPECT_FALSE(s3.isOK()) << "Non-existent edge (2,1) found";
}
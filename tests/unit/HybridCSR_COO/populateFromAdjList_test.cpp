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

// Test populating from adjacency list
TEST_F(HybridStorageShardTest, PopulateFromAdjList_Basic) {
  std::unordered_map<int, std::vector<std::pair<int, int>>,
                     CinderPeak::VertexHasher<int>>
      adj_list;
  adj_list[1] = {{2, 12}, {3, 13}, {4, 14}};
  adj_list[2] = {{3, 23}, {4, 24}};
  adj_list[3] = {{4, 34}};
  adj_list[4] = {};
  adj_list[5] = {{1, 51}, {2, 52}};
  graph->populateFromAdjList(adj_list);
  auto [w12, s12] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s12.isOK()) << "Edge (1,2) not found";
  EXPECT_EQ(w12, 12) << "Incorrect weight for edge (1,2)";
  auto [w23, s23] = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(s23.isOK()) << "Edge (2,3) not found";
  EXPECT_EQ(w23, 23) << "Incorrect weight for edge (2,3)";
  auto [w51, s51] = graph->impl_getEdge(5, 1);
  EXPECT_TRUE(s51.isOK()) << "Edge (5,1) not found";
  EXPECT_EQ(w51, 51) << "Incorrect weight for edge (5,1)";
  auto [w21, s21] = graph->impl_getEdge(2, 1);
  EXPECT_FALSE(s21.isOK()) << "Non-existent edge (2,1) found";
}

// Test populating from empty adjacency list
TEST_F(HybridStorageShardTest, PopulateFromAdjList_Empty) {
  std::unordered_map<int, std::vector<std::pair<int, int>>,
                     CinderPeak::VertexHasher<int>>
      empty_adj_list;
  graph->populateFromAdjList(empty_adj_list);
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(status.isOK()) << "Edge found in empty graph";
}

// Test populating from large adjacency list
TEST_F(HybridStorageShardTest, PopulateFromAdjList_Large) {
  std::unordered_map<int, std::vector<std::pair<int, int>>,
                     CinderPeak::VertexHasher<int>>
      adj_list;
  for (int i = 0; i < 100; ++i) {
    adj_list[i] = {{(i + 1) % 100, i * 10}};
  }
  graph->populateFromAdjList(adj_list);
  for (int i = 0; i < 100; ++i) {
    auto [weight, status] = graph->impl_getEdge(i, (i + 1) % 100);
    EXPECT_TRUE(status.isOK())
        << "Edge (" << i << "," << (i + 1) % 100 << ") not found";
    EXPECT_EQ(weight, i * 10)
        << "Incorrect weight for edge (" << i << "," << (i + 1) % 100 << ")";
  }
}
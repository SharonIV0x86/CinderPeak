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

TEST_F(HybridStorageShardTest, ClearVerticesPrimitive) {
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    EXPECT_TRUE(graph->impl_addVertex(v).isOK());
  }

  EXPECT_TRUE(graph->impl_clearVertices().isOK());

  for (int v : vertices) {
    EXPECT_FALSE(graph->impl_hasVertex(v));
  }
}

TEST_F(HybridStorageShardTest, ClearVerticesString) {
  std::vector<std::string> vertices = {"A", "B", "C", "D", "E"};
  for (std::string v : vertices) {
    EXPECT_TRUE(string_graph->impl_addVertex(v).isOK());
  }

  EXPECT_TRUE(string_graph->impl_clearVertices().isOK());

  for (std::string v : vertices) {
    EXPECT_FALSE(string_graph->impl_hasVertex(v));
  }
}
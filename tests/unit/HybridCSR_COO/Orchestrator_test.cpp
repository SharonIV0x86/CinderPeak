#include "StorageEngine/HybridCSR_COO.hpp"
#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class HybridStorageOrchestratorTest : public ::testing::Test {
protected:
  void SetUp() override { graph = std::make_unique<HybridCSR_COO<int, int>>(); }

  void TearDown() override { graph.reset(); }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
};

TEST_F(HybridStorageOrchestratorTest, RebuildFromAdjList) {
  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      adj_list;
  adj_list[1] = {{2, 10}, {3, 20}};
  adj_list[2] = {{3, 30}};
  adj_list[3] = {};

  graph->orchestrator_rebuildFromAdjList(adj_list);

  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK()) << "Edge (1,2) should exist after rebuild";
  EXPECT_EQ(w1, 10) << "Weight of (1,2) incorrect";

  auto [w2, s2] = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(s2.isOK()) << "Edge (2,3) should exist after rebuild";
  EXPECT_EQ(w2, 30) << "Weight of (2,3) incorrect";

  EXPECT_TRUE(graph->impl_hasVertex(3)) << "Vertex 3 should exist";
}

TEST_F(HybridStorageOrchestratorTest, MergeBuffer) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 100);

  graph->orchestrator_mergeBuffer();

  auto [w, s] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s.isOK()) << "Edge (1,2) should exist after merge";
  EXPECT_EQ(w, 100);
}

TEST_F(HybridStorageOrchestratorTest, ClearAll) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 50);

  graph->orchestrator_clearAll();

  EXPECT_FALSE(graph->impl_hasVertex(1)) << "Vertex 1 should be gone";
  EXPECT_FALSE(graph->impl_hasVertex(2)) << "Vertex 2 should be gone";
  auto [w, s] = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(s.isOK()) << "Edge (1,2) should not exist";
}

TEST_F(HybridStorageOrchestratorTest, BuildIfNeeded) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addEdge(1, 2, 50); // In COO

  graph->orchestrator_buildIfNeeded();

  auto [w, s] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s.isOK()) << "Edge (1,2) should exist";
  EXPECT_EQ(w, 50);
}

TEST_F(HybridStorageOrchestratorTest, SetCOOThreshold) {
  graph->setCOOThreshold(10);
  graph->impl_addVertex(1);
  SUCCEED();
}

TEST_F(HybridStorageOrchestratorTest, ConcurrentMergeAndAdd) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);

  std::atomic<bool> done{false};

  std::thread merger([&]() {
    while (!done.load()) {
      graph->orchestrator_mergeBuffer();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  });

  std::vector<std::thread> writers;
  const int NUM_THREADS = 4;
  const int NUM_OPS = 1000;

  for (int i = 0; i < NUM_THREADS; ++i) {
    writers.emplace_back([&, i]() {
      for (int j = 0; j < NUM_OPS; ++j) {
        graph->impl_addEdge(1, 2, j + (i * 1000));
      }
    });
  }

  for (auto &t : writers) {
    t.join();
  }
  done.store(true);
  merger.join();

  EXPECT_TRUE(graph->impl_hasVertex(1));
  auto [w, s] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s.isOK()) << "Edge should exist after concurrent ops";
}
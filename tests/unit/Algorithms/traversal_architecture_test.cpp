#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Algorithms/TraversalExecution.hpp"
#include "Algorithms/TraversalSemantics.hpp"
#include "Algorithms/TraversalSnapshot.hpp"
#include "CinderGraph.hpp"
#include "GraphRuntime.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "gtest/gtest.h"
#include <vector>

using namespace CinderPeak;

namespace {

constexpr int kRepeatCount = 5;

} // namespace

TEST(TraversalArchitectureTest, RepeatedSnapshotBfsIsIdentical) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addVertex(4);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 4);

  Algorithms::TraversalSnapshot<int, int> snapshot(
      adj.impl_captureTraversalAdjacency());
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  const std::vector<int> expected{1, 2, 3, 4};
  for (int i = 0; i < kRepeatCount; ++i) {
    auto result = alg.bfs(1, snapshot);
    ASSERT_TRUE(result.isOK()) << "iteration " << i;
    EXPECT_EQ(result.order_, expected) << "iteration " << i;
  }
}

TEST(TraversalArchitectureTest, RepeatedLiveBfsIsIdenticalOnUnchangedGraph) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);

  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(),
      [&adj](const int &v) { return adj.impl_hasVertex(v); },
      [&adj](const int &v) { return adj.impl_getNeighbors(v); });

  std::vector<int> baseline;
  for (int i = 0; i < kRepeatCount; ++i) {
    auto result = alg.bfs(1);
    ASSERT_TRUE(result.isOK());
    if (i == 0) {
      baseline = result.order_;
    } else {
      EXPECT_EQ(result.order_, baseline);
    }
  }
}

TEST(TraversalArchitectureTest, SnapshotAdjacencyUnchangedAfterLiveMutation) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addEdge(1, 2);

  Algorithms::TraversalSnapshot<int, int> snapshot(
      adj.impl_captureTraversalAdjacency());
  const auto neighbors_before = snapshot.getNeighbors(1);

  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(2, 3);

  const auto neighbors_after = snapshot.getNeighbors(1);
  EXPECT_EQ(neighbors_before.first, neighbors_after.first);
  EXPECT_EQ(neighbors_before.second.code(), neighbors_after.second.code());
}

TEST(TraversalArchitectureTest, BackendSnapshotParityAfterNormalization) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(1, 2);

  auto amap = adj.impl_captureTraversalAdjacency();
  auto hybrid_ptr = std::make_shared<PeakStore::HybridCSR_COO<int, int>>();
  hybrid_ptr->populateFromAdjList(amap);

  Algorithms::TraversalSnapshot<int, int> adj_snapshot(
      adj.impl_captureTraversalAdjacency());
  Algorithms::TraversalSnapshot<int, int> hybrid_snapshot(
      hybrid_ptr->impl_captureTraversalAdjacency());

  Algorithms::CinderPeakAlgorithms<int, int> alg(hybrid_ptr, nullptr, nullptr);

  auto adj_bfs = alg.bfs(1, adj_snapshot);
  auto hybrid_bfs = alg.bfs(1, hybrid_snapshot);

  EXPECT_TRUE(adj_bfs.isOK());
  EXPECT_TRUE(hybrid_bfs.isOK());
  EXPECT_EQ(adj_bfs.order_, hybrid_bfs.order_);
}

TEST(TraversalArchitectureTest, TraversalAdjacencyAccessMatchesSnapshotBfs) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(10);
  (void)adj.impl_addVertex(20);
  (void)adj.impl_addEdge(10, 20);

  Algorithms::TraversalSnapshot<int, int> snapshot(
      adj.impl_captureTraversalAdjacency());
  auto access =
      Algorithms::TraversalAdjacencyAccess<int, int>::fromSnapshot(snapshot);

  auto direct = Algorithms::runBfs(
      access, 10, Algorithms::TraversalNeighborOrder::DeterministicByVertex);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);
  auto via_alg = alg.bfs(10, snapshot);

  EXPECT_TRUE(direct.isOK());
  EXPECT_TRUE(via_alg.isOK());
  EXPECT_EQ(direct.order_, via_alg.order_);
}

TEST(TraversalArchitectureTest, CustomVertexRepeatedSnapshotParity) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<CinderVertex, int> adj(runtime);
  CinderVertex v1("A"), v2("B"), v3("C");
  (void)adj.impl_addVertex(v1);
  (void)adj.impl_addVertex(v2);
  (void)adj.impl_addVertex(v3);
  (void)adj.impl_addEdge(v1, v2);
  (void)adj.impl_addEdge(v2, v3);

  Algorithms::TraversalSnapshot<CinderVertex, int> snapshot(
      adj.impl_captureTraversalAdjacency());
  Algorithms::CinderPeakAlgorithms<CinderVertex, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<CinderVertex, int>>(), nullptr,
      nullptr);

  std::vector<size_t> baseline_ids;
  for (int i = 0; i < kRepeatCount; ++i) {
    auto result = alg.bfs(v1, snapshot);
    ASSERT_TRUE(result.isOK());
    std::vector<size_t> ids;
    for (const auto &v : result.order_) {
      ids.push_back(v.__id_);
    }
    if (i == 0) {
      baseline_ids = ids;
    } else {
      EXPECT_EQ(ids, baseline_ids);
    }
  }
}

TEST(TraversalArchitectureTest, PeakStoreHybridSnapshotFallbackSemantics) {
  PeakStore::GraphInternalMetadata metadata("arch_test", true, true, false,
                                            true);
  PeakStore::PeakStore<int, int> store(metadata);

  (void)store.addVertex(7);
  (void)store.addVertex(8);
  (void)store.addEdge(7, 8);

  auto hybrid_snapshot = store.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Hybrid);
  auto adj_snapshot = store.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Adjacency);

  ASSERT_NE(hybrid_snapshot, nullptr);
  ASSERT_NE(adj_snapshot, nullptr);

  auto hyb_bfs = store.bfs(7, hybrid_snapshot);
  auto adj_bfs = store.bfs(7, adj_snapshot);
  EXPECT_TRUE(hyb_bfs.isOK());
  EXPECT_TRUE(adj_bfs.isOK());
  EXPECT_EQ(hyb_bfs.order_, adj_bfs.order_);
}

TEST(TraversalArchitectureTest, CinderGraphSnapshotIsolationFromMutation) {
  CinderGraph<int, int> graph;
  (void)graph.addVertex(1);
  (void)graph.addVertex(2);
  (void)graph.addEdge(1, 2, 0);

  auto snapshot = graph.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Adjacency);

  std::vector<int> frozen_order;
  for (int i = 0; i < kRepeatCount; ++i) {
    auto r = graph.bfs(1, snapshot);
    ASSERT_TRUE(r.isOK());
    if (i == 0) {
      frozen_order = r.order_;
    } else {
      EXPECT_EQ(r.order_, frozen_order);
    }
  }

  (void)graph.addVertex(99);
  (void)graph.addEdge(2, 99, 0);
  auto after_mutation = graph.bfs(1, snapshot);
  EXPECT_EQ(after_mutation.order_, frozen_order);
}

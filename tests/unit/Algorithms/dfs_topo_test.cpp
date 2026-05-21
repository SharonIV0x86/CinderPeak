#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Algorithms/TraversalExecution.hpp"
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

Algorithms::TraversalSnapshot<int, int>
makeSnapshot(PeakStore::AdjacencyList<int, int> &adj) {
  return Algorithms::TraversalSnapshot<int, int>(
      adj.impl_captureTraversalAdjacency());
}

} // namespace

TEST(DfsTopoTest, DfsSnapshotCorrectness) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addVertex(4);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 4);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  auto result = alg.dfs(1, snapshot);
  ASSERT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 4, 3}));
}

TEST(DfsTopoTest, DfsRepeatedSnapshotTraversalIsIdentical) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  std::vector<int> baseline;
  for (int i = 0; i < kRepeatCount; ++i) {
    auto result = alg.dfs(1, snapshot);
    ASSERT_TRUE(result.isOK());
    if (i == 0) {
      baseline = result.order_;
    } else {
      EXPECT_EQ(result.order_, baseline);
    }
  }
}

TEST(DfsTopoTest, DfsDisconnectedComponent) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  for (int v : {1, 2, 3, 4, 5}) {
    (void)adj.impl_addVertex(v);
  }
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(2, 3);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  auto result = alg.dfs(1, snapshot);
  ASSERT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3}));
}

TEST(DfsTopoTest, DfsCustomVertexSnapshot) {
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

  auto result = alg.dfs(v1, snapshot);
  ASSERT_TRUE(result.isOK());
  ASSERT_EQ(result.order_.size(), 3u);
  EXPECT_EQ(result.order_[0].__id_, v1.__id_);
  EXPECT_EQ(result.order_[1].__id_, v2.__id_);
  EXPECT_EQ(result.order_[2].__id_, v3.__id_);
}

TEST(DfsTopoTest, DfsSnapshotMatchesTraversalExecution) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addEdge(1, 2);

  auto snapshot = makeSnapshot(adj);
  auto access =
      Algorithms::TraversalAdjacencyAccess<int, int>::fromSnapshot(snapshot);
  auto direct = Algorithms::runDfs(
      access, 1, Algorithms::TraversalNeighborOrder::DeterministicByVertex);

  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);
  auto via_alg = alg.dfs(1, snapshot);

  EXPECT_TRUE(direct.isOK());
  EXPECT_TRUE(via_alg.isOK());
  EXPECT_EQ(direct.order_, via_alg.order_);
}

TEST(DfsTopoTest, DfsBackendSnapshotParity) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(2, 3);

  auto amap = adj.impl_captureTraversalAdjacency();
  auto hybrid_ptr = std::make_shared<PeakStore::HybridCSR_COO<int, int>>();
  hybrid_ptr->populateFromAdjList(amap);

  Algorithms::TraversalSnapshot<int, int> adj_snapshot(
      adj.impl_captureTraversalAdjacency());
  Algorithms::TraversalSnapshot<int, int> hybrid_snapshot(
      hybrid_ptr->impl_captureTraversalAdjacency());

  Algorithms::CinderPeakAlgorithms<int, int> alg(hybrid_ptr, nullptr, nullptr);

  auto adj_dfs = alg.dfs(1, adj_snapshot);
  auto hybrid_dfs = alg.dfs(1, hybrid_snapshot);
  EXPECT_TRUE(adj_dfs.isOK());
  EXPECT_TRUE(hybrid_dfs.isOK());
  EXPECT_EQ(adj_dfs.order_, hybrid_dfs.order_);
}

TEST(DfsTopoTest, TopologicalSortDagCorrectness) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addVertex(4);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 4);
  (void)adj.impl_addEdge(3, 4);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  auto result = alg.topologicalSort(snapshot);
  ASSERT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3, 4}));
}

TEST(DfsTopoTest, TopologicalSortDetectsCycle) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(2, 1);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  auto result = alg.topologicalSort(snapshot);
  EXPECT_FALSE(result.isOK());
  EXPECT_EQ(result._status.code(), StatusCode::GRAPH_CYCLE_DETECTED);
  EXPECT_TRUE(result.order_.empty());
}

TEST(DfsTopoTest, TopologicalSortRepeatedConsistency) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(5);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(8);
  (void)adj.impl_addEdge(5, 2);
  (void)adj.impl_addEdge(2, 8);

  auto snapshot = makeSnapshot(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  std::vector<int> baseline;
  for (int i = 0; i < kRepeatCount; ++i) {
    auto result = alg.topologicalSort(snapshot);
    ASSERT_TRUE(result.isOK());
    if (i == 0) {
      baseline = result.order_;
    } else {
      EXPECT_EQ(result.order_, baseline);
    }
  }
  EXPECT_EQ(baseline, std::vector<int>({5, 2, 8}));
}

TEST(DfsTopoTest, TopologicalSortBackendParity) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 3);

  auto amap = adj.impl_captureTraversalAdjacency();
  auto hybrid_ptr = std::make_shared<PeakStore::HybridCSR_COO<int, int>>();
  hybrid_ptr->populateFromAdjList(amap);

  Algorithms::TraversalSnapshot<int, int> adj_snapshot(
      adj.impl_captureTraversalAdjacency());
  Algorithms::TraversalSnapshot<int, int> hybrid_snapshot(
      hybrid_ptr->impl_captureTraversalAdjacency());

  Algorithms::CinderPeakAlgorithms<int, int> alg(hybrid_ptr, nullptr, nullptr);

  auto adj_topo = alg.topologicalSort(adj_snapshot);
  auto hybrid_topo = alg.topologicalSort(hybrid_snapshot);
  EXPECT_TRUE(adj_topo.isOK());
  EXPECT_TRUE(hybrid_topo.isOK());
  EXPECT_EQ(adj_topo.order_, hybrid_topo.order_);
}

TEST(DfsTopoTest, PeakStoreCinderGraphIntegration) {
  CinderGraph<int, int> graph;
  (void)graph.addVertex(1);
  (void)graph.addVertex(2);
  (void)graph.addVertex(3);
  (void)graph.addEdge(1, 2, 0);
  (void)graph.addEdge(2, 3, 0);

  auto snapshot = graph.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Adjacency);

  auto dfs_result = graph.dfs(1, snapshot);
  ASSERT_TRUE(dfs_result.isOK());
  EXPECT_EQ(dfs_result.order_, std::vector<int>({1, 2, 3}));

  auto topo_result = graph.topologicalSort(snapshot);
  ASSERT_TRUE(topo_result.isOK());
  EXPECT_EQ(topo_result.order_, std::vector<int>({1, 2, 3}));
}

TEST(DfsTopoTest, TopologicalSortRequiresSnapshot) {
  PeakStore::GraphInternalMetadata metadata("topo_test", true, true, false,
                                            true);
  PeakStore::PeakStore<int, int> store(metadata);

  auto result = store.topologicalSort(
      std::shared_ptr<const Algorithms::TraversalSnapshot<int, int>>());
  EXPECT_FALSE(result.isOK());
  EXPECT_EQ(result._status.code(), StatusCode::INVALID_ARGUMENT);
}

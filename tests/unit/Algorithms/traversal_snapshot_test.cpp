#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Algorithms/TraversalSnapshot.hpp"
#include "CinderGraph.hpp"
#include "GraphRuntime.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

namespace {

Algorithms::TraversalSnapshot<int, int>
makeSnapshotFromAdjacency(PeakStore::AdjacencyList<int, int> &adj) {
  return Algorithms::TraversalSnapshot<int, int>(
      adj.impl_captureTraversalAdjacency());
}

} // namespace

TEST(TraversalSnapshotTest, SnapshotBfsMatchesCallbackBfs) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addVertex(4);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 4);

  auto snapshot = makeSnapshotFromAdjacency(adj);

  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(),
      [&adj](const int &v) { return adj.impl_hasVertex(v); },
      [&adj](const int &v) { return adj.impl_getNeighbors(v); });

  auto callback_result = alg.bfs(1);
  auto snapshot_result = alg.bfs(1, snapshot);

  EXPECT_TRUE(callback_result.isOK());
  EXPECT_TRUE(snapshot_result.isOK());
  EXPECT_EQ(callback_result.order_, snapshot_result.order_);
}

TEST(TraversalSnapshotTest, SnapshotImmutabilityAfterMutation) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addEdge(1, 2);

  auto snapshot = makeSnapshotFromAdjacency(adj);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(2, 3);

  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(),
      [&adj](const int &v) { return adj.impl_hasVertex(v); },
      [&adj](const int &v) { return adj.impl_getNeighbors(v); });

  auto frozen = alg.bfs(1, snapshot);
  auto live = alg.bfs(1);

  EXPECT_EQ(frozen.order_, std::vector<int>({1, 2}));
  EXPECT_EQ(live.order_, std::vector<int>({1, 2, 3}));
}

TEST(TraversalSnapshotTest, AdjacencyVsHybridSnapshotParity) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(2, 3);

  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      amap = adj.impl_captureTraversalAdjacency();
  auto hybrid_ptr = std::make_shared<PeakStore::HybridCSR_COO<int, int>>();
  hybrid_ptr->populateFromAdjList(amap);

  auto adj_snapshot = Algorithms::TraversalSnapshot<int, int>(
      adj.impl_captureTraversalAdjacency());
  auto hybrid_snapshot = Algorithms::TraversalSnapshot<int, int>(
      hybrid_ptr->impl_captureTraversalAdjacency());

  Algorithms::CinderPeakAlgorithms<int, int> alg(
      hybrid_ptr, [](const int &) { return true; },
      [](const int &) {
        return std::pair<std::vector<std::pair<int, int>>, PeakStatus>{};
      });

  auto adj_bfs = alg.bfs(1, adj_snapshot);
  auto hybrid_bfs = alg.bfs(1, hybrid_snapshot);

  EXPECT_TRUE(adj_bfs.isOK());
  EXPECT_TRUE(hybrid_bfs.isOK());
  EXPECT_EQ(adj_bfs.order_, hybrid_bfs.order_);
}

TEST(TraversalSnapshotTest, DisconnectedGraphViaSnapshot) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  for (int v : {1, 2, 3, 4, 5}) {
    (void)adj.impl_addVertex(v);
  }
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(2, 3);

  auto snapshot = makeSnapshotFromAdjacency(adj);
  Algorithms::CinderPeakAlgorithms<int, int> alg(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(), nullptr, nullptr);

  auto result = alg.bfs(1, snapshot);
  EXPECT_TRUE(result.isOK());
  EXPECT_EQ(result.order_, std::vector<int>({1, 2, 3}));
}

TEST(TraversalSnapshotTest, CustomVertexSnapshotTraversal) {
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

  auto result = alg.bfs(v1, snapshot);
  EXPECT_TRUE(result.isOK());
  ASSERT_EQ(result.order_.size(), 3u);
  EXPECT_EQ(result.order_[0].__id_, v1.__id_);
  EXPECT_EQ(result.order_[1].__id_, v2.__id_);
  EXPECT_EQ(result.order_[2].__id_, v3.__id_);
}

TEST(TraversalSnapshotTest, PeakStoreSnapshotOrchestration) {
  CinderGraph<int, int> graph;
  (void)graph.addVertex(1);
  (void)graph.addVertex(2);
  (void)graph.addEdge(1, 2, 0);

  auto snapshot = graph.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Adjacency);
  ASSERT_NE(snapshot, nullptr);
  EXPECT_EQ(snapshot->vertexCount(), 2u);

  (void)graph.addVertex(3);
  (void)graph.addEdge(2, 3, 0);

  auto frozen = graph.bfs(1, snapshot);
  auto live = graph.bfs(1);

  EXPECT_EQ(frozen.order_, std::vector<int>({1, 2}));
  EXPECT_EQ(live.order_, std::vector<int>({1, 2, 3}));
}

TEST(TraversalSnapshotTest, EmptyHybridSnapshotFallsBackToAdjacency) {
  PeakStore::GraphInternalMetadata metadata("test_graph", true, true, false,
                                            true);
  PeakStore::PeakStore<int, int> store(metadata);

  (void)store.addVertex(10);
  (void)store.addVertex(20);
  (void)store.addEdge(10, 20);

  auto snapshot = store.createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend::Hybrid);
  ASSERT_NE(snapshot, nullptr);
  EXPECT_TRUE(snapshot->hasVertex(10));
  EXPECT_TRUE(snapshot->hasVertex(20));

  auto neighbors = snapshot->getNeighbors(10);
  ASSERT_TRUE(neighbors.second.isOK());
  ASSERT_EQ(neighbors.first.size(), 1u);
  EXPECT_EQ(neighbors.first[0].first, 20);
}

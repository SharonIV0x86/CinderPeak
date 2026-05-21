#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "GraphRuntime.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

TEST(BFSParityTest, AdjacencyVsHybridPrimitive) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<int, int> adj(runtime);
  PeakStore::HybridCSR_COO<int, int> hybrid;

  // Build graph: 1 -> 2, 1 -> 3, 2 -> 4
  (void)adj.impl_addVertex(1);
  (void)adj.impl_addVertex(2);
  (void)adj.impl_addVertex(3);
  (void)adj.impl_addVertex(4);
  (void)adj.impl_addEdge(1, 2);
  (void)adj.impl_addEdge(1, 3);
  (void)adj.impl_addEdge(2, 4);

  // Prepare adjacency-style map for hybrid population
  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      amap;
  amap[1] = {{2, 0}, {3, 0}};
  amap[2] = {{4, 0}};
  amap[3] = {};
  amap[4] = {};

  // create shared hybrid and populate
  auto hybrid_ptr = std::make_shared<PeakStore::HybridCSR_COO<int, int>>();
  hybrid_ptr->populateFromAdjList(amap);

  // adjacency-only algorithm (no hybrid available)
  Algorithms::CinderPeakAlgorithms<int, int> alg_adj(
      std::shared_ptr<PeakStore::HybridCSR_COO<int, int>>(),
      [&adj](const int &v) { return adj.impl_hasVertex(v); },
      [&adj](const int &v) { return adj.impl_getNeighbors(v); });

  // hybrid-first algorithm (prefers hybrid, falls back to adjacency)
  Algorithms::CinderPeakAlgorithms<int, int> alg_hyb(
      hybrid_ptr,
      [hyb = hybrid_ptr, &adj](const int &v) {
        if (hyb->impl_hasVertex(v))
          return true;
        return adj.impl_hasVertex(v);
      },
      [hyb = hybrid_ptr, &adj](const int &v) {
        auto res = hyb->impl_getNeighbors(v);
        if (res.second.isOK())
          return res;
        return adj.impl_getNeighbors(v);
      });

  auto r1 = alg_adj.bfs(1);
  auto r2 = alg_hyb.bfs(1);

  EXPECT_TRUE(r1.isOK());
  EXPECT_TRUE(r2.isOK());
  EXPECT_EQ(r1.order_, r2.order_);
}

TEST(BFSParityTest, CustomVertexParity) {
  GraphRuntime runtime;
  PeakStore::AdjacencyList<CinderVertex, int> adj(runtime);
  CinderVertex v1("A"), v2("B"), v3("C");
  (void)adj.impl_addVertex(v1);
  (void)adj.impl_addVertex(v2);
  (void)adj.impl_addVertex(v3);
  (void)adj.impl_addEdge(v1, v2);
  (void)adj.impl_addEdge(v2, v3);

  std::unordered_map<CinderVertex, std::vector<std::pair<CinderVertex, int>>,
                     VertexHasher<CinderVertex>>
      amap;
  amap[v1] = {{v2, 0}};
  amap[v2] = {{v3, 0}};
  amap[v3] = {};

  // populate hybrid from adjacency map
  // (handled below via hybrid_ptr->populateFromAdjList)

  Algorithms::CinderPeakAlgorithms<CinderVertex, int> alg_adj(
      std::make_shared<PeakStore::HybridCSR_COO<CinderVertex, int>>(),
      [&adj](const CinderVertex &v) { return adj.impl_hasVertex(v); },
      [&adj](const CinderVertex &v) { return adj.impl_getNeighbors(v); });

  auto hybrid_ptr =
      std::make_shared<PeakStore::HybridCSR_COO<CinderVertex, int>>();
  hybrid_ptr->populateFromAdjList(amap);
  Algorithms::CinderPeakAlgorithms<CinderVertex, int> alg_hyb(
      hybrid_ptr,
      [hyb = hybrid_ptr, &adj](const CinderVertex &v) {
        if (hyb->impl_hasVertex(v))
          return true;
        return adj.impl_hasVertex(v);
      },
      [hyb = hybrid_ptr, &adj](const CinderVertex &v) {
        auto res = hyb->impl_getNeighbors(v);
        if (res.second.isOK())
          return res;
        return adj.impl_getNeighbors(v);
      });

  auto r1 = alg_adj.bfs(v1);
  auto r2 = alg_hyb.bfs(v1);

  EXPECT_TRUE(r1.isOK());
  EXPECT_TRUE(r2.isOK());
  EXPECT_EQ(r1.order_.size(), r2.order_.size());
  // Compare __id_ to avoid relying on string names ordering
  for (size_t i = 0; i < r1.order_.size(); ++i) {
    EXPECT_EQ(r1.order_[i].__id_, r2.order_[i].__id_);
  }
}

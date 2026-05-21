#pragma once
#include "../StorageEngine/Utils.hpp"
#include "Result/bfs_result.hpp"
#include "Result/dfs_result.hpp"
#include "Result/topological_sort_result.hpp"
#include "TraversalExecution.hpp"
#include "TraversalSemantics.hpp"
#include "TraversalSnapshot.hpp"
#include <functional>
#include <memory>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class HybridCSR_COO;

} // namespace PeakStore

// Traversal architecture semantics: see TraversalSemantics.hpp.

namespace Algorithms {

template <typename VertexType, typename EdgeType> class CinderPeakAlgorithms {
public:
  std::shared_ptr<PeakStore::HybridCSR_COO<VertexType, EdgeType>> hcsr =
      nullptr;
  std::function<bool(const VertexType &)> hasVertexFn;
  std::function<std::pair<std::vector<std::pair<VertexType, EdgeType>>,
                          PeakStatus>(const VertexType &)>
      getNeighborsFn;

  CinderPeakAlgorithms(
      const std::shared_ptr<PeakStore::HybridCSR_COO<VertexType, EdgeType>>
          &hybridcsr,
      std::function<bool(const VertexType &)> hasVertex,
      std::function<std::pair<std::vector<std::pair<VertexType, EdgeType>>,
                              PeakStatus>(const VertexType &)>
          getNeighbors)
      : hcsr(hybridcsr), hasVertexFn(std::move(hasVertex)),
        getNeighborsFn(std::move(getNeighbors)) {}

  BFSResult<VertexType> bfs(const VertexType &src) const {
    return runBfs(TraversalAdjacencyAccess<VertexType, EdgeType>::fromCallbacks(
                      hasVertexFn, getNeighborsFn),
                  src, TraversalNeighborOrder::PreserveBackendOrder);
  }

  BFSResult<VertexType>
  bfs(const VertexType &src,
      const TraversalSnapshot<VertexType, EdgeType> &snapshot) const {
    return runBfs(
        TraversalAdjacencyAccess<VertexType, EdgeType>::fromSnapshot(snapshot),
        src, TraversalNeighborOrder::DeterministicByVertex);
  }

  DFSResult<VertexType> dfs(const VertexType &src) const {
    return runDfs(TraversalAdjacencyAccess<VertexType, EdgeType>::fromCallbacks(
                      hasVertexFn, getNeighborsFn),
                  src, TraversalNeighborOrder::PreserveBackendOrder);
  }

  DFSResult<VertexType>
  dfs(const VertexType &src,
      const TraversalSnapshot<VertexType, EdgeType> &snapshot) const {
    return runDfs(
        TraversalAdjacencyAccess<VertexType, EdgeType>::fromSnapshot(snapshot),
        src, TraversalNeighborOrder::DeterministicByVertex);
  }

  TopologicalSortResult<VertexType> topologicalSort(
      const TraversalSnapshot<VertexType, EdgeType> &snapshot) const {
    return runTopologicalSort(snapshot);
  }
};

} // namespace Algorithms
} // namespace CinderPeak

#pragma once
#include "Result/bfs_result.hpp"
#include <iostream>
#include <memory>
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class HybridCSR_COO;

}
namespace Algorithms {
template <typename VertexType, typename EdgeType> class CinderPeakAlgorithms {
public:
  std::shared_ptr<PeakStore::HybridCSR_COO<VertexType, EdgeType>> hcsr =
      nullptr;
  CinderPeakAlgorithms(
      const std::shared_ptr<PeakStore::HybridCSR_COO<VertexType, EdgeType>>
          &hcsr)
      : hcsr(hcsr) {};
  BFSResult<VertexType> bfs(const VertexType &src) {
    std::cout << "Algorithms::bfs called\n";

    BFSResult<VertexType> result;
    result.order_.push_back(1);
    result.order_.push_back(2);
    result.order_.push_back(3);
    result.order_.push_back(4);
    return result;
  }
};
} // namespace Algorithms
} // namespace CinderPeak
#pragma once

#include <iostream>
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
  void bfs() { std::cout << "Hello from bfs()\n"; }
};
} // namespace Algorithms
} // namespace CinderPeak
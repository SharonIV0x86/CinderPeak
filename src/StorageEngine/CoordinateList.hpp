#pragma once
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <memory>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class CoordinateList {
public:
  std::shared_ptr<GraphContext<VertexType, EdgeType>> ctx;
  CoordinateList(const std::shared_ptr<GraphContext<VertexType, EdgeType>> &ctx)
      : ctx(ctx) {}
  CoordinateList() {}
  std::vector<VertexType> coo_src;
  std::vector<VertexType> coo_dest;
  std::vector<EdgeType> coo_weights;
};
} // namespace PeakStore

} // namespace CinderPeak

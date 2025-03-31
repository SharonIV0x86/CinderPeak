#pragma once
#include "Utils.hpp"
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class AdjacencyList {
    public:
  std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;
  std::shared_ptr<GraphCreationOptions> create_options = nullptr;

  AdjacencyList(const std::shared_ptr<GraphInternalMetadata> &metadata,
                const std::shared_ptr<GraphCreationOptions> &options)
      : graph_metadata(metadata), create_options(options) {}
};
} // namespace PeakStore

} // namespace CinderPeak

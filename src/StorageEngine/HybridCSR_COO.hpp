#pragma once
#include "Utils.hpp"

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class HybridCSR_COO {
public:
  std::shared_ptr<GraphCreationOptions> graph_options = nullptr;
  std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;

  HybridCSR_COO(const std::shared_ptr<GraphInternalMetadata> &metadata,
                const std::shared_ptr<GraphCreationOptions> &options)
      : graph_metadata(metadata), graph_options(options) {}

  bool hasOption(GraphCreationOptions::GraphType opt) const {
    return graph_options && graph_options->hasOption(opt);
  }
};
} // namespace PeakStore

} // namespace CinderPeak

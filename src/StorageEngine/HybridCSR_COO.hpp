#pragma once
#include "Utils.hpp"

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class HybridCSR_COO {
public:
  std::unique_ptr<GraphCreationOptions> graph_options = nullptr;
  std::unique_ptr<GraphInternalMetadata> graph_metadata = nullptr;
  HybridCSR_COO(const GraphCreationOptions &create_options,
                const GraphInternalMetadata &metadata) {
    graph_options = std::make_unique<GraphCreationOptions>(create_options);
    graph_metadata = std::make_unique<GraphInternalMetadata>(metadata);
  }

  bool hasOption(GraphType opt) const {
    return graph_options && graph_options->hasOption(opt);
  }
};
} // namespace PeakStore

} // namespace CinderPeak

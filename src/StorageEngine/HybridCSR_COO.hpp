#pragma once
#include "Utils.hpp"

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class HybridCSR_COO {
public:
  std::shared_ptr<GraphCreationOptions> graph_options = nullptr;
  std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;
  std::vector<size_t> csr_row_offsets;
  std::vector<const VertexType *> csr_col_ptrs;
  std::vector<const EdgeType *> csr_weight_ptrs;

  std::vector<const VertexType *> coo_src_ptrs;
  std::vector<const VertexType *> coo_dest_ptrs;
  std::vector<const EdgeType *> coo_weight_ptrs;

  HybridCSR_COO(const std::shared_ptr<GraphInternalMetadata> &metadata,
                const std::shared_ptr<GraphCreationOptions> &options)
      : graph_metadata(metadata), graph_options(options) {}
  void impl_buildStructures(
      const std::unique_ptr<AdjacencyList<VertexType, EdgeType>> &adj_storage) {
    const size_t num_vertices = adj_storage->graph_metadata->num_vertices;

    csr_row_offsets.clear();
    csr_col_ptrs.clear();
    csr_weight_ptrs.clear();

    csr_row_offsets.resize(num_vertices + 1, 0);
    size_t total_edges = 0;

    for (size_t v = 0; v < num_vertices; ++v) {
      csr_row_offsets[v] = total_edges;
      // total_edges += adj_storage.
    }
  }
  bool hasOption(GraphCreationOptions::GraphType opt) const {
    return graph_options && graph_options->hasOption(opt);
  }
};
} // namespace PeakStore

} // namespace CinderPeak

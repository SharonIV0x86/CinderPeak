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

  std::vector<std::tuple<const VertexType*, const VertexType*, const EdgeType*>> PBuf_pending_additions_w;
  std::vector<std::tuple<const VertexType*, const VertexType*, const EdgeType*>> PBuf_pending_additions;
  std::vector<std::tuple<const VertexType*, const VertexType*>> PBuf_pending_deletions;


  HybridCSR_COO(const std::shared_ptr<GraphInternalMetadata> &metadata,
                const std::shared_ptr<GraphCreationOptions> &options)
      : graph_metadata(metadata), graph_options(options) {}
  void impl_buildStructures(
      const std::unique_ptr<AdjacencyList<VertexType, EdgeType>>
          &adj_storage_obj) {
    const size_t num_vertices = adj_storage_obj->graph_metadata->num_vertices;
    csr_row_offsets.clear();
    csr_col_ptrs.clear();
    csr_weight_ptrs.clear();
    coo_src_ptrs.clear();
    coo_dest_ptrs.clear();
    coo_weight_ptrs.clear();
    csr_row_offsets.resize(num_vertices + 1, 0);

    const auto &adj_list = adj_storage_obj->getAdjList();
    size_t total_edges = 0;
    size_t row = 0;

    for (const auto &[src, neighbors] : adj_list) {
      csr_row_offsets[row] = total_edges;

      for (const auto &[dest, weight] : neighbors) {
        csr_col_ptrs.push_back(&dest);
        csr_weight_ptrs.push_back(&weight);
        coo_src_ptrs.push_back(&src);
        coo_dest_ptrs.push_back(&dest);
        coo_weight_ptrs.push_back(&weight);
        ++total_edges;
      }
      ++row;
    }
    csr_row_offsets[row] = total_edges; 
  }
  void impl_addEdgePointer(const VertexType* src, const VertexType* dest, const EdgeType* weight){
    PBuf_pending_additions_w.emplace_back(src, dest, weight);
  }
  void impl_addEdgePointer(const VertexType* src, const VertexType* dest){
    PBuf_pending_additions.emplace_back(src, dest);
  }
  //we dont have to worry about the weightedness here as the PeakStore will be an mediatery and will call
  //this method appropriately only if the graph is weighted.
  void impl_removeEdgePointer(const VertexType* src, const VertexType* dest){
    PBuf_pending_deletions.emplace_back(src, dest);
  }
  bool hasOption(GraphCreationOptions::GraphType opt) const {
    return graph_options && graph_options->hasOption(opt);
  }
};
} // namespace PeakStore

} // namespace CinderPeak

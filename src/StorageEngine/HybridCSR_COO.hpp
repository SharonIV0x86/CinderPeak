#pragma once
#include "../StorageInterface.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>
namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;
namespace PeakStore {

template <typename VertexType, typename EdgeType>
class HybridCSR_COO : public PeakStorageInterface<VertexType, EdgeType> {
private:
  alignas(64) std::vector<size_t> csr_row_offsets;
  alignas(64) std::vector<VertexType> csr_col_vals;
  alignas(64) std::vector<EdgeType> csr_weights;

  // Added alignment to COO arrays for consistency
  alignas(64) std::vector<VertexType> coo_src;
  alignas(64) std::vector<VertexType> coo_dest;
  alignas(64) std::vector<EdgeType> coo_weights;

  std::vector<VertexType> vertex_order;
  std::unordered_map<VertexType, size_t, VertexHasher<VertexType>>
      vertex_to_index;

  bool is_built{false};
  size_t COO_BUFFER_THRESHOLD{1024}; // Threshold for COO Buffer

  void buildStructures() {
    if (is_built)
      return;
    is_built = true;

    const size_t num_vertices = vertex_order.size();
    csr_row_offsets.assign(num_vertices + 1, 0);
    csr_col_vals.clear();
    csr_weights.clear();

    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (vertex_to_index.count(coo_src[i])) {
        size_t idx = vertex_to_index[coo_src[i]];
        csr_row_offsets[idx + 1]++;
      }
    }

    for (size_t i = 1; i <= num_vertices; ++i) {
      csr_row_offsets[i] += csr_row_offsets[i - 1];
    }

    csr_col_vals.resize(csr_row_offsets[num_vertices]);
    csr_weights.resize(csr_row_offsets[num_vertices]);

    std::vector<size_t> insert_offsets = csr_row_offsets;
    std::vector<std::vector<std::pair<VertexType, EdgeType>>> temp_rows(
        num_vertices);
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (vertex_to_index.count(coo_src[i]) &&
          vertex_to_index.count(coo_dest[i])) {
        size_t row = vertex_to_index[coo_src[i]];
        temp_rows[row].emplace_back(coo_dest[i], coo_weights[i]);
      }
    }

    for (size_t row = 0; row < num_vertices; ++row) {
      std::sort(temp_rows[row].begin(), temp_rows[row].end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });
      for (const auto &[dest, weight] : temp_rows[row]) {
        size_t pos = insert_offsets[row]++;
        csr_col_vals[pos] = dest;
        csr_weights[pos] = weight;
      }
    }

    clearCOOArrays(); // Used helper function instead of inline clearing
  }

  void incrementalUpdate() {
    if (!is_built || coo_src.empty())
      return;

    const size_t num_vertices = vertex_order.size();
    std::vector<size_t> new_edge_counts(num_vertices, 0);

    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (vertex_to_index.count(coo_src[i])) {
        new_edge_counts[vertex_to_index[coo_src[i]]]++;
      }
    }

    std::vector<size_t> new_row_offsets(num_vertices + 1, 0);
    new_row_offsets[0] = csr_row_offsets[0];
    for (size_t i = 0; i < num_vertices; ++i) {
      new_row_offsets[i + 1] = new_row_offsets[i] +
                               (csr_row_offsets[i + 1] - csr_row_offsets[i]) +
                               new_edge_counts[i];
    }

    std::vector<VertexType> new_col_vals(new_row_offsets.back());
    std::vector<EdgeType> new_weights(new_row_offsets.back());

    std::vector<size_t> insert_offsets = new_row_offsets;
    for (size_t row = 0; row < num_vertices; ++row) {
      VertexType src = vertex_order[row];
      size_t old_start = csr_row_offsets[row];
      size_t old_end = csr_row_offsets[row + 1];
      std::vector<std::pair<VertexType, EdgeType>> merged_neighbors;

      for (size_t i = old_start; i < old_end; ++i) {
        merged_neighbors.emplace_back(csr_col_vals[i], csr_weights[i]);
      }

      for (size_t i = 0; i < coo_src.size(); ++i) {
        if (coo_src[i] == src && vertex_to_index.count(coo_dest[i])) {
          merged_neighbors.emplace_back(coo_dest[i], coo_weights[i]);
        }
      }

      std::sort(merged_neighbors.begin(), merged_neighbors.end(),
                [](const auto &a, const auto &b) { return a.first < b.first; });

      for (const auto &[dest, weight] : merged_neighbors) {
        size_t pos = insert_offsets[row]++;
        new_col_vals[pos] = dest;
        new_weights[pos] = weight;
      }
    }

    csr_row_offsets = std::move(new_row_offsets);
    csr_col_vals = std::move(new_col_vals);
    csr_weights = std::move(new_weights);

    clearCOOArrays(); // Used helper function instead of inline clearing
  }

  // Added helper function to clear and shrink COO arrays to reduce redundacy
  void clearCOOArrays() {
    coo_src.clear();
    coo_dest.clear();
    coo_weights.clear();
    coo_src.shrink_to_fit();
    coo_dest.shrink_to_fit();
    coo_weights.shrink_to_fit();
  }

public:
  HybridCSR_COO() {
    csr_row_offsets.reserve(1024);
    csr_col_vals.reserve(4096);
    csr_weights.reserve(4096);
    coo_src.reserve(4096);
    coo_dest.reserve(4096);
    coo_weights.reserve(4096);
    vertex_order.reserve(1024);
    vertex_to_index.reserve(1024);
  }

  void populateFromAdjList(
      const std::unordered_map<VertexType,
                               std::vector<std::pair<VertexType, EdgeType>>,
                               VertexHasher<VertexType>> &adj_list) {
    is_built = false;
    clearCOOArrays(); // Used helper function instead of inline clearing
    vertex_order.clear();
    vertex_to_index.clear();

    for (const auto &[src, neighbors] : adj_list) {
      if (!vertex_to_index.count(src)) {
        vertex_to_index[src] = vertex_order.size();
        vertex_order.push_back(src);
      }
      for (const auto &[dest, weight] : neighbors) {
        if (!vertex_to_index.count(dest)) {
          vertex_to_index[dest] = vertex_order.size();
          vertex_order.push_back(dest);
        }
        coo_src.push_back(src);
        coo_dest.push_back(dest);
        coo_weights.push_back(weight);
      }
    }

    buildStructures();
  }

  void exc() const {
    std::cout << "HybridCSR_COO CSR:\n";
    for (size_t i = 0; i < vertex_order.size(); ++i) {
      std::cout << vertex_order[i] << " -> ";
      for (size_t j = csr_row_offsets[i]; j < csr_row_offsets[i + 1]; ++j) {
        std::cout << "(" << csr_col_vals[j] << ", " << csr_weights[j] << ") ";
      }
      std::cout << "\n";
    }
  }

  const PeakStatus impl_addVertex(const VertexType &vtx) override {
    if (vertex_to_index.count(vtx)) {
      return PeakStatus::AlreadyExists();
    }
    size_t new_idx = vertex_order.size();
    vertex_to_index[vtx] = new_idx;
    vertex_order.push_back(vtx);
    if (is_built) {
      csr_row_offsets.push_back(csr_row_offsets.back());
    }
    return PeakStatus::OK();
  }

  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                const EdgeType &weight) override {
    if (!vertex_to_index.count(src) || !vertex_to_index.count(dest)) {
      return PeakStatus::VertexNotFound();
    }
    coo_src.push_back(src);
    coo_dest.push_back(dest);
    coo_weights.push_back(weight);
    if (is_built &&
        coo_src.size() >= COO_BUFFER_THRESHOLD) { // Batched incremental updates
                                                  // based on threshold value
      incrementalUpdate();
    }
    return PeakStatus::OK();
  }

  const PeakStatus impl_addEdge(const VertexType &src,
                                const VertexType &dest) override {
    return impl_addEdge(src, dest, EdgeType{});
  }

  bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) override {
    auto edge = impl_getEdge(src, dest);
    return edge.second.isOK() && edge.first == weight;
  }

  bool impl_doesEdgeExist(const VertexType &src,
                          const VertexType &dest) override {
    return impl_getEdge(src, dest).second.isOK();
  }

  const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {

    // First check COO buffer for recent edges (most recent edges have priority)
    for (size_t i = coo_src.size(); i > 0; --i) {
      if (coo_src[i - 1] == src && coo_dest[i - 1] == dest) {
        return {coo_weights[i - 1], PeakStatus::OK()};
      }
    }

    if (!is_built) {
      buildStructures();
    }
    if (!vertex_to_index.count(src) || !vertex_to_index.count(dest)) {
      return {EdgeType{}, PeakStatus::VertexNotFound()};
    }
    size_t row = vertex_to_index.at(src);
    size_t start = csr_row_offsets[row];
    size_t end = csr_row_offsets[row + 1];

    auto it = std::lower_bound(csr_col_vals.begin() + start,
                               csr_col_vals.begin() + end, dest);
    if (it != csr_col_vals.begin() + end && *it == dest) {
      size_t idx = std::distance(csr_col_vals.begin(), it);
      return {csr_weights[idx], PeakStatus::OK()};
    }
    return {EdgeType{}, PeakStatus::EdgeNotFound()};
  }
};

} // namespace PeakStore
} // namespace CinderPeak
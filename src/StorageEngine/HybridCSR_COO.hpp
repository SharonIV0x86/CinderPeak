#pragma once

#include "engine/GraphContext.hpp"
#include "storage/interface/StorageInterface.hpp"
#include "storage/utils/Utils.hpp"

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace CinderPeak {
template <typename, typename> class PeakStorageInterface;

namespace PeakStore {

template <typename VertexType, typename EdgeType>
class HybridCSR_COO : public PeakStorageInterface<VertexType, EdgeType> {
private:
  using diff_t = std::ptrdiff_t;

  static inline diff_t to_diff(size_t x) { return static_cast<diff_t>(x); }

  alignas(64) std::vector<size_t> csr_row_offsets;
  alignas(64) std::vector<size_t> csr_col_vals;
  alignas(64) std::vector<EdgeType> csr_weights;

  alignas(64) std::vector<size_t> coo_src;
  alignas(64) std::vector<size_t> coo_dest;
  alignas(64) std::vector<EdgeType> coo_weights;

  std::vector<VertexType> vertex_order;
  std::unordered_map<VertexType, size_t, VertexHasher<VertexType>>
      vertex_to_index;

  // Built-state row storage. Each row is ordered by destination vertex id.
  std::vector<std::map<size_t, EdgeType>> csr_rows_;

  mutable std::shared_mutex _mtx;
  mutable std::atomic<bool> is_built_{false};
  std::atomic<size_t> COO_BUFFER_THRESHOLD_{1024};

  bool csr_dirty_{true};

  void ensureRowStorageLocked() {
    if (csr_rows_.size() != vertex_order.size()) {
      csr_rows_.resize(vertex_order.size());
    }
  }

  void clearCOOArrays() noexcept {
    coo_src.clear();
    coo_dest.clear();
    coo_weights.clear();
  }

  void mergeBufferedEdgesIntoRowsLocked() {
    if (coo_src.empty()) {
      return;
    }

    ensureRowStorageLocked();

    const size_t n = vertex_order.size();
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] < n && coo_dest[i] < n) {
        csr_rows_[coo_src[i]][coo_dest[i]] = coo_weights[i];
      }
    }

    clearCOOArrays();
    csr_dirty_ = true;
  }

  void rebuildCSRSnapshotLocked() {
    ensureRowStorageLocked();

    const size_t n = vertex_order.size();
    csr_row_offsets.assign(n + 1, 0);
    csr_col_vals.clear();
    csr_weights.clear();

    size_t total_edges = 0;
    for (size_t row = 0; row < n; ++row) {
      total_edges += csr_rows_[row].size();
      csr_row_offsets[row + 1] = total_edges;
    }

    csr_col_vals.resize(total_edges);
    csr_weights.resize(total_edges);

    size_t pos = 0;
    for (size_t row = 0; row < n; ++row) {
      for (const auto &[dest_idx, weight] : csr_rows_[row]) {
        csr_col_vals[pos] = dest_idx;
        csr_weights[pos] = weight;
        ++pos;
      }
    }

    csr_dirty_ = false;
  }

  void buildStructuresLocked() {
    mergeBufferedEdgesIntoRowsLocked();
    rebuildCSRSnapshotLocked();
    is_built_.store(true, std::memory_order_release);
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
    csr_rows_.reserve(1024);
  }

  void buildStructures() {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (is_built_.load(std::memory_order_acquire) && !csr_dirty_ &&
        coo_src.empty()) {
      return;
    }

    buildStructuresLocked();
  }

  void incrementalUpdate() {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (!is_built_.load(std::memory_order_relaxed) || coo_src.empty()) {
      return;
    }

    mergeBufferedEdgesIntoRowsLocked();
    // Snapshot rebuild is deferred; lookups/updates use csr_rows_ directly.
  }

  void populateFromAdjList(
      const std::unordered_map<VertexType,
                               std::vector<std::pair<VertexType, EdgeType>>,
                               VertexHasher<VertexType>> &adj_list) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    is_built_.store(false, std::memory_order_relaxed);
    csr_dirty_ = true;

    clearCOOArrays();
    vertex_order.clear();
    vertex_to_index.clear();
    csr_rows_.clear();
    csr_row_offsets.clear();
    csr_col_vals.clear();
    csr_weights.clear();

    for (const auto &[src, neighbors] : adj_list) {
      if (vertex_to_index.find(src) == vertex_to_index.end()) {
        vertex_to_index[src] = vertex_order.size();
        vertex_order.push_back(src);
      }

      for (const auto &[dest, weight] : neighbors) {
        (void)weight;
        if (vertex_to_index.find(dest) == vertex_to_index.end()) {
          vertex_to_index[dest] = vertex_order.size();
          vertex_order.push_back(dest);
        }
      }
    }

    csr_rows_.assign(vertex_order.size(), {});

    for (const auto &[src, neighbors] : adj_list) {
      const size_t src_idx = vertex_to_index[src];
      for (const auto &[dest, weight] : neighbors) {
        const size_t dest_idx = vertex_to_index[dest];
        csr_rows_[src_idx][dest_idx] = weight;
      }
    }

    rebuildCSRSnapshotLocked();
    is_built_.store(true, std::memory_order_release);
  }

  void setCOOThreshold(size_t threshold) {
    COO_BUFFER_THRESHOLD_.store(threshold, std::memory_order_relaxed);
  }

  void orchestrator_rebuildFromAdjList(
      const std::unordered_map<VertexType,
                               std::vector<std::pair<VertexType, EdgeType>>,
                               VertexHasher<VertexType>> &adj_list) {
    populateFromAdjList(adj_list);
  }

  void orchestrator_mergeBuffer() { incrementalUpdate(); }

  void orchestrator_clearAll() { impl_clearVertices(); }

  void orchestrator_buildIfNeeded() { buildStructures(); }

  void exc() const {
    std::shared_lock<std::shared_mutex> lock(_mtx);

    std::cout << "HybridCSR_COO CSR (Indices):\n";

    if (csr_dirty_ || csr_row_offsets.size() != vertex_order.size() + 1) {
      // Debug output only; rebuild lazily for a correct snapshot.
      lock.unlock();
      const_cast<HybridCSR_COO *>(this)->buildStructures();
      lock.lock();
    }

    for (size_t i = 0; i < vertex_order.size(); ++i) {
      std::cout << vertex_order[i] << " [" << i << "] -> ";
      for (size_t j = csr_row_offsets[i]; j < csr_row_offsets[i + 1]; ++j) {
        const size_t neighbor_idx = csr_col_vals[j];
        std::cout << "(" << vertex_order[neighbor_idx] << " [" << neighbor_idx
                  << "], " << csr_weights[j] << ") ";
      }
      std::cout << "\n";
    }
  }

  [[nodiscard]] const PeakStatus
  impl_addVertex(const VertexType &vtx) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto vtx_it = vertex_to_index.find(vtx);
    if (vtx_it != vertex_to_index.end()) {
      return PeakStatus::AlreadyExists();
    }

    const size_t new_idx = vertex_order.size();
    vertex_to_index[vtx] = new_idx;
    vertex_order.push_back(vtx);
    csr_rows_.emplace_back();

    if (is_built_.load(std::memory_order_relaxed)) {
      csr_dirty_ = true;
    }

    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus
  impl_addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight = EdgeType()) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto src_it = vertex_to_index.find(src);
    auto dest_it = vertex_to_index.find(dest);
    if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end()) {
      return PeakStatus::VertexNotFound();
    }

    const size_t src_idx = src_it->second;
    const size_t dest_idx = dest_it->second;

    if (is_built_.load(std::memory_order_relaxed)) {
      ensureRowStorageLocked();
      csr_rows_[src_idx][dest_idx] = weight;
      csr_dirty_ = true;
      return PeakStatus::OK();
    }

    coo_src.push_back(src_idx);
    coo_dest.push_back(dest_idx);
    coo_weights.push_back(weight);

    if (coo_src.size() >=
        COO_BUFFER_THRESHOLD_.load(std::memory_order_relaxed)) {
      lock.unlock();
      buildStructures();
    }

    return PeakStatus::OK();
  }

  [[nodiscard]] const std::pair<EdgeType, PeakStatus>
  impl_removeEdge(const VertexType &src, const VertexType &dest) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    EdgeType weight{};

    auto src_it = vertex_to_index.find(src);
    auto dest_it = vertex_to_index.find(dest);
    if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end()) {
      return {weight, PeakStatus::VertexNotFound()};
    }

    const size_t src_idx = src_it->second;
    const size_t dest_idx = dest_it->second;

    if (!is_built_.load(std::memory_order_acquire)) {
      for (size_t i = coo_src.size(); i > 0; --i) {
        const size_t idx = i - 1;
        if (coo_src[idx] == src_idx && coo_dest[idx] == dest_idx) {
          weight = coo_weights[idx];
          coo_src.erase(coo_src.begin() + to_diff(idx));
          coo_dest.erase(coo_dest.begin() + to_diff(idx));
          coo_weights.erase(coo_weights.begin() + to_diff(idx));
          return {weight, PeakStatus::OK()};
        }
      }
      return {weight, PeakStatus::EdgeNotFound()};
    }

    ensureRowStorageLocked();
    auto &row = csr_rows_[src_idx];
    auto it = row.find(dest_idx);
    if (it == row.end()) {
      return {weight, PeakStatus::EdgeNotFound()};
    }

    weight = it->second;
    row.erase(it);
    csr_dirty_ = true;
    return {weight, PeakStatus::OK()};
  }

  [[nodiscard]] const PeakStatus
  impl_updateEdge(const VertexType &src, const VertexType &dest,
                  const EdgeType &newWeight) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto src_it = vertex_to_index.find(src);
    auto dest_it = vertex_to_index.find(dest);
    if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end()) {
      return PeakStatus::VertexNotFound();
    }

    const size_t src_idx = src_it->second;
    const size_t dest_idx = dest_it->second;

    if (!is_built_.load(std::memory_order_acquire)) {
      for (size_t i = coo_src.size(); i > 0; --i) {
        const size_t idx = i - 1;
        if (coo_src[idx] == src_idx && coo_dest[idx] == dest_idx) {
          coo_weights[idx] = newWeight;
          return PeakStatus::OK();
        }
      }
      return PeakStatus::EdgeNotFound();
    }

    ensureRowStorageLocked();
    auto &row = csr_rows_[src_idx];
    auto it = row.find(dest_idx);
    if (it == row.end()) {
      return PeakStatus::EdgeNotFound();
    }

    it->second = newWeight;
    csr_dirty_ = true;
    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus impl_clearVertices() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    clearCOOArrays();
    vertex_order.clear();
    vertex_to_index.clear();
    csr_rows_.clear();

    csr_row_offsets.clear();
    csr_col_vals.clear();
    csr_weights.clear();

    csr_dirty_ = true;
    is_built_.store(false, std::memory_order_relaxed);

    return PeakStatus::OK();
  }

  [[nodiscard]] const PeakStatus impl_clearEdges() override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    clearCOOArrays();
    for (auto &row : csr_rows_) {
      row.clear();
    }

    if (is_built_.load(std::memory_order_relaxed)) {
      rebuildCSRSnapshotLocked();
    } else {
      csr_row_offsets.assign(vertex_order.size() + 1, 0);
      csr_col_vals.clear();
      csr_weights.clear();
      csr_dirty_ = false;
    }

    return PeakStatus::OK();
  }

  [[nodiscard]] bool impl_hasVertex(const VertexType &v) noexcept override {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return vertex_to_index.find(v) != vertex_to_index.end();
  }

  [[nodiscard]] bool
  impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                     const EdgeType &weight) noexcept override {
    auto edge = impl_getEdge(src, dest);
    return edge.second.isOK() && edge.first == weight;
  }

  [[nodiscard]] bool
  impl_doesEdgeExist(const VertexType &src,
                     const VertexType &dest) noexcept override {
    return impl_getEdge(src, dest).second.isOK();
  }

  [[nodiscard]] const std::pair<EdgeType, PeakStatus>
  impl_getEdge(const VertexType &src, const VertexType &dest) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    EdgeType weight{};

    auto src_it = vertex_to_index.find(src);
    auto dest_it = vertex_to_index.find(dest);
    if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end()) {
      return {weight, PeakStatus::VertexNotFound()};
    }

    const size_t src_idx = src_it->second;
    const size_t dest_idx = dest_it->second;

    if (!is_built_.load(std::memory_order_acquire)) {
      for (size_t i = coo_src.size(); i > 0; --i) {
        const size_t idx = i - 1;
        if (coo_src[idx] == src_idx && coo_dest[idx] == dest_idx) {
          return {coo_weights[idx], PeakStatus::OK()};
        }
      }
      return {weight, PeakStatus::EdgeNotFound()};
    }

    ensureRowStorageLocked();
    auto &row = csr_rows_[src_idx];
    auto it = row.find(dest_idx);
    if (it != row.end()) {
      return {it->second, PeakStatus::OK()};
    }

    return {weight, PeakStatus::EdgeNotFound()};
  }

  [[nodiscard]] const PeakStatus
  impl_removeVertex(const VertexType &vtx) override {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    auto vtx_it = vertex_to_index.find(vtx);
    if (vtx_it == vertex_to_index.end()) {
      return PeakStatus::VertexNotFound();
    }

    const size_t idx_to_remove = vtx_it->second;

    // Remove any buffered COO edges and renumber remaining indices.
    for (size_t i = 0; i < coo_src.size();) {
      if (coo_src[i] == idx_to_remove || coo_dest[i] == idx_to_remove) {
        coo_src.erase(coo_src.begin() + to_diff(i));
        coo_dest.erase(coo_dest.begin() + to_diff(i));
        coo_weights.erase(coo_weights.begin() + to_diff(i));
      } else {
        if (coo_src[i] > idx_to_remove) {
          --coo_src[i];
        }
        if (coo_dest[i] > idx_to_remove) {
          --coo_dest[i];
        }
        ++i;
      }
    }

    if (is_built_.load(std::memory_order_relaxed)) {
      ensureRowStorageLocked();

      if (idx_to_remove < csr_rows_.size()) {
        csr_rows_.erase(csr_rows_.begin() + to_diff(idx_to_remove));
      }

      for (auto &row : csr_rows_) {
        std::map<size_t, EdgeType> remapped;
        for (const auto &[dest_idx, weight] : row) {
          if (dest_idx == idx_to_remove) {
            continue;
          }
          const size_t new_dest_idx =
              (dest_idx > idx_to_remove) ? (dest_idx - 1) : dest_idx;
          remapped.emplace(new_dest_idx, weight);
        }
        row.swap(remapped);
      }

      csr_dirty_ = true;
    }

    vertex_order.erase(vertex_order.begin() + to_diff(idx_to_remove));

    vertex_to_index.clear();
    for (size_t i = 0; i < vertex_order.size(); ++i) {
      vertex_to_index[vertex_order[i]] = i;
    }

    return PeakStatus::OK();
  }
};

} // namespace PeakStore
} // namespace CinderPeak
#pragma once
#include "../StorageInterface.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace CinderPeak
{
  template <typename, typename>
  class PeakStorageInterface;
  namespace PeakStore
  {

    template <typename VertexType, typename EdgeType>
    class HybridCSR_COO : public PeakStorageInterface<VertexType, EdgeType>
    {
    private:
      alignas(64) std::vector<size_t> csr_row_offsets;
      alignas(64) std::vector<size_t> csr_col_vals;
      alignas(64) std::vector<EdgeType> csr_weights;

      alignas(64) std::vector<size_t> coo_src;
      alignas(64) std::vector<size_t> coo_dest;
      alignas(64) std::vector<EdgeType> coo_weights;

      std::vector<VertexType> vertex_order;
      std::unordered_map<VertexType, size_t, VertexHasher<VertexType>>
          vertex_to_index;

      mutable std::shared_mutex _mtx;
      mutable std::atomic<bool> is_built_{false};
      std::atomic<size_t> COO_BUFFER_THRESHOLD_{1024};

      void buildStructures()
      {
        if (is_built_.load(std::memory_order_acquire))
          return;

        std::unique_lock<std::shared_mutex> lock(_mtx);

        if (is_built_.load(std::memory_order_relaxed))
          return;

        const size_t num_vertices = vertex_order.size();
        csr_row_offsets.assign(num_vertices + 1, 0);
        csr_col_vals.clear();
        csr_weights.clear();

        for (size_t i = 0; i < coo_src.size(); ++i)
        {
          if (coo_src[i] < num_vertices)
          {
            csr_row_offsets[coo_src[i] + 1]++;
          }
        }

        for (size_t i = 1; i <= num_vertices; ++i)
        {
          csr_row_offsets[i] += csr_row_offsets[i - 1];
        }

        csr_col_vals.resize(csr_row_offsets[num_vertices]);
        csr_weights.resize(csr_row_offsets[num_vertices]);

        std::vector<size_t> insert_offsets = csr_row_offsets;
        std::vector<std::vector<std::pair<size_t, EdgeType>>> temp_rows(
            num_vertices);
        for (size_t i = 0; i < coo_src.size(); ++i)
        {
          if (coo_src[i] < num_vertices && coo_dest[i] < num_vertices)
          {
            temp_rows[coo_src[i]].emplace_back(coo_dest[i], coo_weights[i]);
          }
        }

        for (size_t row = 0; row < num_vertices; ++row)
        {
          std::sort(temp_rows[row].begin(), temp_rows[row].end(),
                    [](const auto &a, const auto &b)
                    { return a.first < b.first; });
          for (const auto &[dest_idx, weight] : temp_rows[row])
          {
            size_t pos = insert_offsets[row]++;
            csr_col_vals[pos] = dest_idx;
            csr_weights[pos] = weight;
          }
        }

        clearCOOArrays();
        is_built_.store(true, std::memory_order_release);
      }

      void incrementalUpdate()
      {
        if (!is_built_.load(std::memory_order_relaxed) || coo_src.empty())
          return;

        const size_t num_vertices = vertex_order.size();
        std::vector<size_t> new_edge_counts(num_vertices, 0);

        for (size_t i = 0; i < coo_src.size(); ++i)
        {
          if (coo_src[i] < num_vertices)
          {
            new_edge_counts[coo_src[i]]++;
          }
        }

        std::vector<size_t> new_row_offsets(num_vertices + 1, 0);
        new_row_offsets[0] = csr_row_offsets[0];
        for (size_t i = 0; i < num_vertices; ++i)
        {
          new_row_offsets[i + 1] = new_row_offsets[i] +
                                   (csr_row_offsets[i + 1] - csr_row_offsets[i]) +
                                   new_edge_counts[i];
        }

        std::vector<size_t> new_col_vals(new_row_offsets.back());
        std::vector<EdgeType> new_weights(new_row_offsets.back());

        std::vector<size_t> insert_offsets = new_row_offsets;
        for (size_t row = 0; row < num_vertices; ++row)
        {
          size_t old_start = csr_row_offsets[row];
          size_t old_end = csr_row_offsets[row + 1];
          std::vector<std::pair<size_t, EdgeType>> merged_neighbors;

          for (size_t i = old_start; i < old_end; ++i)
          {
            merged_neighbors.emplace_back(csr_col_vals[i], csr_weights[i]);
          }

          for (size_t i = 0; i < coo_src.size(); ++i)
          {
            if (coo_src[i] == row && coo_dest[i] < num_vertices)
            {
              merged_neighbors.emplace_back(coo_dest[i], coo_weights[i]);
            }
          }

          std::sort(merged_neighbors.begin(), merged_neighbors.end(),
                    [](const auto &a, const auto &b)
                    { return a.first < b.first; });

          for (const auto &[dest_idx, weight] : merged_neighbors)
          {
            size_t pos = insert_offsets[row]++;
            new_col_vals[pos] = dest_idx;
            new_weights[pos] = weight;
          }
        }

        csr_row_offsets = std::move(new_row_offsets);
        csr_col_vals = std::move(new_col_vals);
        csr_weights = std::move(new_weights);

        clearCOOArrays();
      }

      void clearCOOArrays()
      {
        coo_src.clear();
        coo_dest.clear();
        coo_weights.clear();
        coo_src.shrink_to_fit();
        coo_dest.shrink_to_fit();
        coo_weights.shrink_to_fit();
      }

    public:
      HybridCSR_COO()
      {
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
                                   VertexHasher<VertexType>> &adj_list)
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);

        is_built_.store(false, std::memory_order_relaxed);
        clearCOOArrays();
        vertex_order.clear();
        vertex_to_index.clear();

        for (const auto &[src, neighbors] : adj_list)
        {
          auto src_it = vertex_to_index.find(src);
          if (src_it == vertex_to_index.end())
          {
            vertex_to_index[src] = vertex_order.size();
            vertex_order.push_back(src);
          }
          for (const auto &[dest, weight] : neighbors)
          {
            auto dest_it = vertex_to_index.find(dest);
            if (dest_it == vertex_to_index.end())
            {
              vertex_to_index[dest] = vertex_order.size();
              vertex_order.push_back(dest);
            }
          }
        }

        for (const auto &[src, neighbors] : adj_list)
        {
          size_t src_idx = vertex_to_index[src];
          for (const auto &[dest, weight] : neighbors)
          {
            size_t dest_idx = vertex_to_index[dest];
            coo_src.push_back(src_idx);
            coo_dest.push_back(dest_idx);
            coo_weights.push_back(weight);
          }
        }

        lock.unlock();
        buildStructures();
      }

      void setCOOThreshold(size_t threshold)
      {
        COO_BUFFER_THRESHOLD_.store(threshold, std::memory_order_relaxed);
      }

      void orchestrator_rebuildFromAdjList(
          const std::unordered_map<VertexType,
                                   std::vector<std::pair<VertexType, EdgeType>>,
                                   VertexHasher<VertexType>> &adj_list)
      {
        populateFromAdjList(adj_list);
      }

      void orchestrator_mergeBuffer()
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);
        incrementalUpdate();
      }

      void orchestrator_clearAll() { impl_clearVertices(); }

      void orchestrator_buildIfNeeded() { buildStructures(); }

      void exc() const
      {
        std::shared_lock<std::shared_mutex> lock(_mtx);

        std::cout << "HybridCSR_COO CSR (Indices):\n";
        for (size_t i = 0; i < vertex_order.size(); ++i)
        {
          std::cout << vertex_order[i] << " [" << i << "] -> ";
          for (size_t j = csr_row_offsets[i]; j < csr_row_offsets[i + 1]; ++j)
          {
            size_t neighbor_idx = csr_col_vals[j];
            std::cout << "(" << vertex_order[neighbor_idx] << " [" << neighbor_idx
                      << "], " << csr_weights[j] << ") ";
          }
          std::cout << "\n";
        }
      }

      const PeakStatus impl_addVertex(const VertexType &vtx) override
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);

        auto vtx_it = vertex_to_index.find(vtx);
        if (vtx_it != vertex_to_index.end())
        {
          return PeakStatus::AlreadyExists();
        }
        size_t new_idx = vertex_order.size();
        vertex_to_index[vtx] = new_idx;
        vertex_order.push_back(vtx);
        if (is_built_.load(std::memory_order_relaxed))
        {
          csr_row_offsets.push_back(csr_row_offsets.back());
        }
        return PeakStatus::OK();
      }

      const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                    const EdgeType &weight = EdgeType()) override
      {

        std::unique_lock<std::shared_mutex> lock(_mtx);

        auto src_it = vertex_to_index.find(src);
        auto dest_it = vertex_to_index.find(dest);
        if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end())
        {
          return PeakStatus::VertexNotFound();
        }

        coo_src.push_back(src_it->second);
        coo_dest.push_back(dest_it->second);
        coo_weights.push_back(weight);

        if (is_built_.load(std::memory_order_relaxed) &&
            coo_src.size() >=
                COO_BUFFER_THRESHOLD_.load(std::memory_order_relaxed))
        {
          incrementalUpdate();
        }
        return PeakStatus::OK();
      }

      const std::pair<EdgeType, PeakStatus>
      impl_removeEdge(const VertexType &src, const VertexType &dest) override
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);

        auto weight = EdgeType();

        auto src_it = vertex_to_index.find(src);
        auto dest_it = vertex_to_index.find(dest);
        if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end())
        {
          return std::make_pair(weight, PeakStatus::VertexNotFound());
        }

        size_t src_idx = src_it->second;
        size_t dest_idx = dest_it->second;

        for (size_t i = coo_src.size(); i > 0; --i)
        {
          if (coo_src[i - 1] == src_idx && coo_dest[i - 1] == dest_idx)
          {
            coo_src.erase(coo_src.begin() + (i - 1));
            coo_dest.erase(coo_dest.begin() + (i - 1));
            weight = coo_weights[i - 1];
            coo_weights.erase(coo_weights.begin() + (i - 1));
            return std::make_pair(weight, PeakStatus::OK());
          }
        }

        if (!is_built_.load(std::memory_order_acquire))
        {
          return std::make_pair(weight, PeakStatus::EdgeNotFound());
        }

        size_t row = src_idx;
        size_t start = csr_row_offsets[row];
        size_t end = csr_row_offsets[row + 1];

        auto it = std::lower_bound(csr_col_vals.begin() + start,
                                   csr_col_vals.begin() + end, dest_idx);

        if (it != csr_col_vals.begin() + end && *it == dest_idx)
        {
          size_t edge_idx = std::distance(csr_col_vals.begin(), it);

          weight = csr_weights[edge_idx];

          csr_col_vals.erase(csr_col_vals.begin() + edge_idx);
          csr_weights.erase(csr_weights.begin() + edge_idx);

          for (size_t i = row + 1; i < csr_row_offsets.size(); ++i)
          {
            csr_row_offsets[i]--;
          }

          return std::make_pair(weight, PeakStatus::OK());
        }
        return std::make_pair(weight, PeakStatus::EdgeNotFound());
      }

      const PeakStatus impl_updateEdge(const VertexType &src,
                                       const VertexType &dest,
                                       const EdgeType &newWeight) override
      {
        auto src_it = vertex_to_index.find(src);
        auto dest_it = vertex_to_index.find(dest);
        if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end())
        {
          return PeakStatus::VertexNotFound();
        }

        std::unique_lock<std::shared_mutex> lock(_mtx);

        size_t src_idx = src_it->second;
        size_t dest_idx = dest_it->second;

        for (size_t i = coo_src.size(); i > 0; --i)
        {
          if (coo_src[i - 1] == src_idx && coo_dest[i - 1] == dest_idx)
          {
            coo_weights[i - 1] = newWeight;
            return PeakStatus::OK();
          }
        }

        if (!is_built_.load(std::memory_order_relaxed))
        {
          lock.unlock();
          buildStructures();
          lock.lock();
        }

        size_t row = src_idx;
        size_t start = csr_row_offsets[row];
        size_t end = csr_row_offsets[row + 1];

        auto it = std::lower_bound(csr_col_vals.begin() + start,
                                   csr_col_vals.begin() + end, dest_idx);

        if (it != csr_col_vals.begin() + end && *it == dest_idx)
        {
          size_t idx = std::distance(csr_col_vals.begin(), it);
          csr_weights[idx] = newWeight;
          return PeakStatus::OK();
        }

        return PeakStatus::EdgeNotFound();
      }

      const PeakStatus impl_clearVertices() override
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);
        clearCOOArrays();

        if (is_built_.load(std::memory_order_relaxed))
        {
          csr_row_offsets.clear();
          csr_col_vals.clear();
          csr_weights.clear();
          csr_col_vals.shrink_to_fit();
          csr_weights.shrink_to_fit();
        }

        vertex_order.clear();
        vertex_to_index.clear();

        is_built_.store(false, std::memory_order_relaxed);

        return PeakStatus::OK();
      }

      const PeakStatus impl_clearEdges() override
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);
        clearCOOArrays();

        if (is_built_.load(std::memory_order_relaxed))
        {
          std::fill(csr_row_offsets.begin(), csr_row_offsets.end(), 0);
          csr_col_vals.clear();
          csr_weights.clear();
          csr_col_vals.shrink_to_fit();
          csr_weights.shrink_to_fit();
        }

        return PeakStatus::OK();
      }

      bool impl_hasVertex(const VertexType &v) override
      {
        std::shared_lock<std::shared_mutex> lock(_mtx);
        if (!vertex_to_index.count(v))
        {
          return false;
        }
        return true;
      }

      bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                              const EdgeType &weight) override
      {
        auto edge = impl_getEdge(src, dest);
        return edge.second.isOK() && edge.first == weight;
      }

      bool impl_doesEdgeExist(const VertexType &src,
                              const VertexType &dest) override
      {
        return impl_getEdge(src, dest).second.isOK();
      }

      const std::pair<EdgeType, PeakStatus>
      impl_getEdge(const VertexType &src, const VertexType &dest) override
      {
        std::shared_lock<std::shared_mutex> lock(_mtx);

        auto src_it = vertex_to_index.find(src);
        auto dest_it = vertex_to_index.find(dest);
        if (src_it == vertex_to_index.end() || dest_it == vertex_to_index.end())
        {
          return {EdgeType{}, PeakStatus::VertexNotFound()};
        }

        size_t src_idx = src_it->second;
        size_t dest_idx = dest_it->second;

        for (size_t i = coo_src.size(); i > 0; --i)
        {
          if (coo_src[i - 1] == src_idx && coo_dest[i - 1] == dest_idx)
          {
            return {coo_weights[i - 1], PeakStatus::OK()};
          }
        }

        if (!is_built_.load(std::memory_order_acquire))
        {
          lock.unlock();
          buildStructures();
          lock.lock();
        }

        size_t row = src_idx;
        size_t start = csr_row_offsets[row];
        size_t end = csr_row_offsets[row + 1];

        auto it = std::lower_bound(csr_col_vals.begin() + start,
                                   csr_col_vals.begin() + end, dest_idx);
        if (it != csr_col_vals.begin() + end && *it == dest_idx)
        {
          size_t idx = std::distance(csr_col_vals.begin(), it);
          return {csr_weights[idx], PeakStatus::OK()};
        }
        return {EdgeType{}, PeakStatus::EdgeNotFound()};
      }

      const PeakStatus impl_removeVertex(const VertexType &vtx) override
      {
        std::unique_lock<std::shared_mutex> lock(_mtx);

        auto vtx_it = vertex_to_index.find(vtx);
        if (vtx_it == vertex_to_index.end())
        {
          return PeakStatus::VertexNotFound();
        }

        size_t idx_to_remove = vtx_it->second;

        for (size_t i = 0; i < coo_src.size();)
        {
          if (coo_src[i] == idx_to_remove || coo_dest[i] == idx_to_remove)
          {
            coo_src.erase(coo_src.begin() + i);
            coo_dest.erase(coo_dest.begin() + i);
            coo_weights.erase(coo_weights.begin() + i);
          }
          else
          {
            if (coo_src[i] > idx_to_remove)
              coo_src[i]--;
            if (coo_dest[i] > idx_to_remove)
              coo_dest[i]--;
            ++i;
          }
        }

        if (is_built_.load(std::memory_order_relaxed))
        {
          std::vector<size_t> new_csr_cols;
          std::vector<EdgeType> new_csr_weights;
          std::vector<size_t> new_row_offsets(vertex_order.size(), 0);

          size_t current_offset = 0;
          size_t new_row_idx = 0;

          new_row_offsets[0] = 0;

          for (size_t old_row = 0; old_row < vertex_order.size(); ++old_row)
          {
            if (old_row == idx_to_remove)
              continue;

            size_t start = csr_row_offsets[old_row];
            size_t end = csr_row_offsets[old_row + 1];

            for (size_t j = start; j < end; ++j)
            {
              size_t neighbor_idx = csr_col_vals[j];
              if (neighbor_idx != idx_to_remove)
              {
                size_t new_neighbor_idx = (neighbor_idx > idx_to_remove)
                                              ? neighbor_idx - 1
                                              : neighbor_idx;
                new_csr_cols.push_back(new_neighbor_idx);
                new_csr_weights.push_back(csr_weights[j]);
                current_offset++;
              }
            }
            new_row_idx++;
            new_row_offsets[new_row_idx] = current_offset;
          }

          csr_row_offsets = std::move(new_row_offsets);
          csr_col_vals = std::move(new_csr_cols);
          csr_weights = std::move(new_csr_weights);
        }

        vertex_order.erase(vertex_order.begin() + idx_to_remove);

        vertex_to_index.clear();
        for (size_t i = 0; i < vertex_order.size(); ++i)
        {
          vertex_to_index[vertex_order[i]] = i;
        }

        return PeakStatus::OK();
      }
    };

  } // namespace PeakStore
} // namespace CinderPeak
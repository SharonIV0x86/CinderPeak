#pragma once
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

namespace CinderPeak
{
  namespace PeakStore
  {

    template <typename VertexType, typename EdgeType>
    class HybridCSR_COO : public PeakStorageInterface<VertexType, EdgeType>
    {
    private:
      std::vector<size_t> csr_row_offsets;
      std::vector<VertexType> csr_col_vals;
      std::vector<EdgeType> csr_weights;

      std::vector<VertexType> coo_srcs;
      std::vector<VertexType> coo_dests;
      std::vector<EdgeType> coo_weights;

      std::vector<VertexType> vertex_order;
      std::unordered_map<VertexType, size_t, VertexHasher<VertexType>> vertex_to_index;

      std::optional<std::unordered_map<std::pair<VertexType, VertexType>, size_t,
                                       PairHasher<VertexType, VertexType>>>
          edge_index;

      bool is_built{false};

    public:
      HybridCSR_COO() { LOG_INFO("Initialized HybridCSR_COO object"); }

      const PeakStatus impl_addVertex(const VertexType &vtx) override
      {
        if (vertex_to_index.count(vtx))
        {
          return PeakStatus::AlreadyExists();
        }
        vertex_to_index[vtx] = vertex_order.size();
        vertex_order.push_back(vtx);
        return PeakStatus::OK();
      }

      const PeakStatus impl_removeVertex(const VertexType &vtx)
      {
        if (!vertex_to_index.count(vtx))
        {
          return PeakStatus::VertexNotFound();
        }
        is_built = false;
        edge_index.reset();
        coo_srcs.clear();
        coo_dests.clear();
        coo_weights.clear();
        csr_row_offsets.clear();
        csr_col_vals.clear();
        csr_weights.clear();

        size_t idx = vertex_to_index[vtx];
        vertex_order.erase(vertex_order.begin() + idx);
        vertex_to_index.clear();
        for (size_t i = 0; i < vertex_order.size(); ++i)
        {
          vertex_to_index[vertex_order[i]] = i;
        }
        return PeakStatus::OK();
      }

      const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                                    const EdgeType &weight) override
      {
        if (!vertex_to_index.count(src) || !vertex_to_index.count(dest))
        {
          return PeakStatus::VertexNotFound();
        }
        if (is_built)
        {
          return PeakStatus::MethodNotImplemented(); 
        }
        coo_srcs.push_back(src);
        coo_dests.push_back(dest);
        coo_weights.push_back(weight);
        return PeakStatus::OK();
      }

      const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest) override
      {
        return impl_addEdge(src, dest, EdgeType{});
      }

      bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest,
                              const EdgeType &weight) override
      {
        auto edge = impl_getEdge(src, dest);
        return edge.second.isOK() && edge.first == weight;
      }

      bool impl_doesEdgeExist(const VertexType &src, const VertexType &dest) override
      {
        return impl_getEdge(src, dest).second.isOK();
      }

      const std::pair<EdgeType, PeakStatus> impl_getEdge(const VertexType &src,
                                                         const VertexType &dest) override
      {
        if (!is_built)
        {
          return {EdgeType{}, PeakStatus::InternalError("CSR not built")};
        }
        if (!vertex_to_index.count(src) || !vertex_to_index.count(dest))
        {
          return {EdgeType{}, PeakStatus::VertexNotFound()};
        }
        size_t row = vertex_to_index.at(src);
        size_t start = csr_row_offsets[row];
        size_t end = csr_row_offsets[row + 1];

        auto it = std::lower_bound(csr_col_vals.begin() + start, csr_col_vals.begin() + end, dest);
        if (it != csr_col_vals.begin() + end && *it == dest)
        {
          size_t idx = std::distance(csr_col_vals.begin(), it);
          return {csr_weights[idx], PeakStatus::OK()};
        }
        return {EdgeType{}, PeakStatus::EdgeNotFound()};
      }

      void buildStructures(const std::vector<VertexType> &coo_srcs,
                           const std::vector<VertexType> &coo_dests,
                           const std::vector<EdgeType> &coo_weights)
      {
        this->coo_srcs = coo_srcs;
        this->coo_dests = coo_dests;
        this->coo_weights = coo_weights;
        buildStructures();
      }

      void buildStructures()
      {
        if (is_built)
          return;
        is_built = true;
        edge_index.emplace();

        const size_t num_vertices = vertex_order.size();
        const size_t num_edges = coo_srcs.size();
        csr_row_offsets.assign(num_vertices + 1, 0);
        csr_col_vals.clear();
        csr_weights.clear();

        for (size_t i = 0; i < num_edges; ++i)
        {
          if (!vertex_to_index.count(coo_srcs[i]) || !vertex_to_index.count(coo_dests[i]))
          {
            continue;
          }
          size_t idx = vertex_to_index[coo_srcs[i]];
          csr_row_offsets[idx + 1]++;
        }

        for (size_t i = 1; i <= num_vertices; ++i)
        {
          csr_row_offsets[i] += csr_row_offsets[i - 1];
        }

        csr_col_vals.resize(csr_row_offsets[num_vertices]);
        csr_weights.resize(csr_row_offsets[num_vertices]);

        std::vector<size_t> insert_offsets = csr_row_offsets;

        std::vector<std::vector<std::pair<VertexType, EdgeType>>> temp_rows(num_vertices);
        for (size_t i = 0; i < num_edges; ++i)
        {
          if (!vertex_to_index.count(coo_srcs[i]) || !vertex_to_index.count(coo_dests[i]))
          {
            continue;
          }
          size_t row = vertex_to_index[coo_srcs[i]];
          temp_rows[row].emplace_back(coo_dests[i], coo_weights[i]);
        }

        for (size_t row = 0; row < num_vertices; ++row)
        {
          std::sort(temp_rows[row].begin(), temp_rows[row].end(),
                    [](const auto &a, const auto &b)
                    { return a.first < b.first; });
          for (const auto &[dest, weight] : temp_rows[row])
          {
            size_t pos = insert_offsets[row]++;
            csr_col_vals[pos] = dest;
            csr_weights[pos] = weight;
            edge_index->emplace(std::make_pair(vertex_order[row], dest), pos);
          }
        }
        coo_srcs.clear();
        coo_dests.clear();
        coo_weights.clear();
        coo_srcs.shrink_to_fit();
        coo_dests.shrink_to_fit();
        coo_weights.shrink_to_fit();
      }

      struct NeighborIterator
      {
        const VertexType *col_begin;
        const VertexType *col_end;
        const EdgeType *weight_begin;

        bool hasNext() const { return col_begin != col_end; }
        std::pair<VertexType, EdgeType> next()
        {
          auto result = std::make_pair(*col_begin, *weight_begin);
          ++col_begin;
          ++weight_begin;
          return result;
        }
      };

      NeighborIterator getNeighbors(const VertexType &vtx) const
      {
        if (!is_built || !vertex_to_index.count(vtx))
        {
          return {nullptr, nullptr, nullptr};
        }
        size_t row = vertex_to_index.at(vtx);
        size_t start = csr_row_offsets[row];
        size_t end = csr_row_offsets[row + 1];
        __builtin_prefetch(&csr_col_vals[start + 32 / sizeof(VertexType)]); 
        return {&csr_col_vals[start], &csr_col_vals[end], &csr_weights[start]};
      }

      void exc() const
      {
        std::cout << "CSR Built:\n";
        for (size_t i = 0; i < vertex_order.size(); ++i)
        {
          std::cout << vertex_order[i] << " -> ";
          for (size_t j = csr_row_offsets[i]; j < csr_row_offsets[i + 1]; ++j)
          {
            std::cout << "(" << csr_col_vals[j] << ", " << csr_weights[j] << ") ";
          }
          std::cout << "\n";
        }
      }
    };

  } // namespace PeakStore
} // namespace CinderPeak
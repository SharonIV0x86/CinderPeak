#pragma once
#include "StorageEngine/GraphContext.hpp"
// #include "StorageInterface.hpp"
#include "Utils.hpp"
#include <memory>
namespace CinderPeak
{
  template <typename VertexType, typename EgdeType>
  class PeakStorageInterface;
  namespace PeakStore
  {
    template <typename VertexType, typename EdgeType>
    class HybridCSR_COO : public CinderPeak::PeakStorageInterface<VertexType, EdgeType>
    {
    private:
      std::vector<size_t> csr_row_offsets;
      std::vector<VertexType> csr_col_vals;
      std::vector<EdgeType> csr_weights;

      std::vector<VertexType> vertex_order;
      std::unordered_map<VertexType, size_t, VertexHasher<VertexType>> vertex_to_index;

    public:
      HybridCSR_COO() = default;

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

        size_t idx = vertex_to_index[vtx];
        vertex_order.erase(vertex_order.begin() + idx);
        vertex_to_index.clear();

        for (size_t i = 0; i < vertex_order.size(); ++i)
        {
          vertex_to_index[vertex_order[i]] = i;
        }

        return PeakStatus::OK();
      }
      const PeakStatus impl_addEdge(const VertexType& src, const VertexType& dest)override{
        LOG_WARNING("Called an Unimplemented method");
        return PeakStatus::MethodNotImplemented();
      } 
      const PeakStatus impl_addEdge(const VertexType& src, const VertexType& dest, const EdgeType &weight) override{
        LOG_WARNING("Called an Unimplemented method");
        return PeakStatus::MethodNotImplemented();
      }
      void buildStructures(const std::vector<VertexType> &coo_srcs,
                           const std::vector<VertexType> &coo_dests,
                           const std::vector<EdgeType> &coo_weights)
      {
        const size_t num_vertices = vertex_order.size();
        csr_row_offsets.assign(num_vertices + 1, 0);
        csr_col_vals.clear();
        csr_weights.clear();

        for (const auto &src : coo_srcs)
        {
          if (vertex_to_index.count(src))
          {
            size_t idx = vertex_to_index[src];
            csr_row_offsets[idx + 1]++;
          }
        }

        for (size_t i = 1; i <= num_vertices; ++i)
        {
          csr_row_offsets[i] += csr_row_offsets[i - 1];
        }

        // Allocate space for column + weight
        size_t total_edges = coo_srcs.size();
        csr_col_vals.resize(total_edges);
        csr_weights.resize(total_edges);

        // Temp array to track current write positions
        std::vector<size_t> insert_offsets = csr_row_offsets;

        for (size_t i = 0; i < total_edges; ++i)
        {
          const VertexType &src = coo_srcs[i];
          const VertexType &dest = coo_dests[i];
          const EdgeType &weight = coo_weights[i];

          if (!vertex_to_index.count(src))
            continue;

          size_t row = vertex_to_index[src];
          size_t pos = insert_offsets[row]++;

          csr_col_vals[pos] = dest;
          csr_weights[pos] = weight;
        }
      }

      void exc() const
      {
        std::cout << "CSR Built: \n";
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

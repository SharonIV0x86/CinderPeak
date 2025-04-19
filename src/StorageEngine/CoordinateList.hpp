#pragma once
#include "StorageEngine/GraphContext.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <memory>
#include <unordered_set>

namespace CinderPeak {
  template <typename, typename>
class PeakStorageInterface;
namespace PeakStore {
template <typename VertexType, typename EdgeType> 
class CoordinateList : public CinderPeak::PeakStorageInterface<VertexType, EdgeType> {
private:
  std::vector<VertexType> coo_src;
  std::vector<VertexType> coo_dest;
  std::vector<EdgeType> coo_weights;
  std::unordered_set<VertexType, VertexHasher<VertexType>> vertices;

public:
  CoordinateList(){
    impl_BuildCoordinateList();
  }
  void impl_BuildCoordinateList() {
    coo_src.clear();
    coo_dest.clear();
    coo_weights.clear();
    vertices.clear();
  }
  const PeakStatus impl_addVertex(const VertexType &vertex) {
    auto [_, inserted] = vertices.insert(vertex);
    if (!inserted) {
        return PeakStatus::AlreadyExists();
    }
    return PeakStatus::OK();
  }

  PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest) {
    EdgeType defaultWeight = EdgeType();
    return impl_addEdge(src, dest, defaultWeight);
  }

  PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) {
    vertices.insert(src);
    vertices.insert(dest);

    coo_src.push_back(src);
    coo_dest.push_back(dest);
    coo_weights.push_back(weight);

    return PeakStatus::OK();
  }

  std::pair<PeakStatus, EdgeType> impl_getEdge(const VertexType &src,
                                               const VertexType &dest) {
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src && coo_dest[i] == dest) {
        return {PeakStatus::OK(), coo_weights[i]};
      }
    }
    return {PeakStatus::NotFound("Edge not found"), EdgeType()};
  }

  std::pair<PeakStatus, std::vector<std::pair<VertexType, EdgeType>>>
  impl_getOutEdges(const VertexType &src) {
    std::vector<std::pair<VertexType, EdgeType>> edges;
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src) {
        edges.emplace_back(coo_dest[i], coo_weights[i]);
      }
    }
    return {PeakStatus::OK(), edges};
  }
  std::pair<PeakStatus, std::vector<VertexType>> impl_getAllVertices() {
    std::vector<VertexType> vertexList(vertices.begin(), vertices.end());
    return {PeakStatus::OK(), vertexList};
  }
  std::pair<PeakStatus,
            std::vector<std::tuple<VertexType, VertexType, EdgeType>>>
  impl_getAllEdges() {
    std::vector<std::tuple<VertexType, VertexType, EdgeType>> edges;
    for (size_t i = 0; i < coo_src.size(); ++i) {
      edges.emplace_back(coo_src[i], coo_dest[i], coo_weights[i]);
    }
    return {PeakStatus::OK(), edges};
  }
  std::pair<PeakStatus, bool> impl_hasVertex(const VertexType &vertex) {
    return {PeakStatus::OK(), vertices.find(vertex) != vertices.end()};
  }
  std::pair<PeakStatus, bool> impl_hasEdge(const VertexType &src,
                                           const VertexType &dest) {
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src && coo_dest[i] == dest) {
        return {PeakStatus::OK(), true};
      }
    }
    return {PeakStatus::OK(), false};
  }

  PeakStatus impl_removeVertex(const VertexType &vertex) {
    size_t i = 0;
    while (i < coo_src.size()) {
      if (coo_src[i] == vertex || coo_dest[i] == vertex) {
        coo_src.erase(coo_src.begin() + i);
        coo_dest.erase(coo_dest.begin() + i);
        coo_weights.erase(coo_weights.begin() + i);
      } else {
        ++i;
      }
    }

    vertices.erase(vertex);
    return PeakStatus::OK();
  }

  PeakStatus impl_removeEdge(const VertexType &src, const VertexType &dest) {
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src && coo_dest[i] == dest) {
        coo_src.erase(coo_src.begin() + i);
        coo_dest.erase(coo_dest.begin() + i);
        coo_weights.erase(coo_weights.begin() + i);
        return PeakStatus::OK();
      }
    }
    return PeakStatus::NotFound("Edge not found");
  }

  std::pair<PeakStatus, size_t> impl_getVertexCount() {
    return {PeakStatus::OK(), vertices.size()};
  }
  std::pair<PeakStatus, size_t> impl_getEdgeCount() {
    return {PeakStatus::OK(), coo_src.size()};
  }
  std::pair<PeakStatus, size_t> impl_getOutDegree(const VertexType &src) {
    size_t count = 0;
    for (const auto &v : coo_src) {
      if (v == src)
        count++;
    }
    return {PeakStatus::OK(), count};
  }
  std::pair<PeakStatus, size_t> impl_getInDegree(const VertexType &dest) {
    size_t count = 0;
    for (const auto &v : coo_dest) {
      if (v == dest)
        count++;
    }
    return {PeakStatus::OK(), count};
  }
};
} // namespace PeakStore
} // namespace CinderPeak
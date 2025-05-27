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
    LOG_INFO("Inside COO:BuildCoordinateList");
    coo_src.clear();
    coo_dest.clear();
    coo_weights.clear();
    vertices.clear();
  }
  const PeakStatus impl_addVertex(const VertexType &vertex) override {
    LOG_INFO("Inside COO:addVertex");
    auto [_, inserted] = vertices.insert(vertex);
    if (!inserted) {
        return PeakStatus::AlreadyExists();
    }
    return PeakStatus::OK();
  }
  bool impl_doesEdgeExist(const VertexType& src, const VertexType& dest, const EdgeType& weight)  override {
    LOG_WARNING("Called unimplemented doesEdgeExist");
    return false;
  }
  bool impl_doesEdgeExist(const VertexType& src, const VertexType& dest)  override {
    LOG_WARNING("Called unimplemented doesEdgeExist");
    return false;
  }

  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest) override {
    LOG_INFO("Inside unweighted COO:addEdge");
    vertices.insert(src);
    vertices.insert(dest);

    coo_src.push_back(src);
    coo_dest.push_back(dest);
    // Add default weight
    coo_weights.push_back(EdgeType());//  this has to be changed later for complex types or primitives, ow lawd.
    return PeakStatus::OK();
}

  const PeakStatus impl_addEdge(const VertexType &src, const VertexType &dest,
                          const EdgeType &weight) override {

    LOG_INFO("Inside weighted COO:addEdge");
    vertices.insert(src);
    vertices.insert(dest);

    coo_src.push_back(src);
    coo_dest.push_back(dest);
    coo_weights.push_back(weight);

    return PeakStatus::OK();
  }

  const std::pair<EdgeType, PeakStatus> impl_getEdge(const VertexType &src,
                                               const VertexType &dest) override{
    
    LOG_INFO("Inside COO:getEdge");
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src && coo_dest[i] == dest) {
        return {coo_weights[i], PeakStatus::OK()};
      }
    }
    return {EdgeType(), PeakStatus::NotFound("Edge not found")};
  }

  std::pair<PeakStatus, std::vector<std::pair<VertexType, EdgeType>>>
  impl_getOutEdges(const VertexType &src) {
    LOG_INFO("Inside  COO:getOutEdges");
    std::vector<std::pair<VertexType, EdgeType>> edges;
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src) {
        edges.emplace_back(coo_dest[i], coo_weights[i]);
      }
    }
    return {PeakStatus::OK(), edges};
  }
  std::pair<PeakStatus, std::vector<VertexType>> impl_getAllVertices() {
    LOG_INFO("Inside COO:getAllVertices");
    std::vector<VertexType> vertexList(vertices.begin(), vertices.end());
    return {PeakStatus::OK(), vertexList};
  }
  std::pair<PeakStatus,
            std::vector<std::tuple<VertexType, VertexType, EdgeType>>>
  impl_getAllEdges() {
    LOG_INFO("Inside COO:getAllEdges");
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
   LOG_INFO("Inside COO:hasEdge");
    for (size_t i = 0; i < coo_src.size(); ++i) {
      if (coo_src[i] == src && coo_dest[i] == dest) {
        return {PeakStatus::OK(), true};
      }
    }
    return {PeakStatus::OK(), false};
  }

  PeakStatus impl_removeVertex(const VertexType &vertex) {
    LOG_INFO("Inside COO:removeVertex");
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
    LOG_INFO("Inside COO:removeEdge");
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
    LOG_INFO("Inside COO:getVertexCount");
    return {PeakStatus::OK(), vertices.size()};
  }
  std::pair<PeakStatus, size_t> impl_getEdgeCount() {
    LOG_INFO("Inside COO:getEdgeCount");
    return {PeakStatus::OK(), coo_src.size()};
  }
  std::pair<PeakStatus, size_t> impl_getOutDegree(const VertexType &src) {
    LOG_INFO("Inside COO:getOutDegree");
    size_t count = 0;
    for (const auto &v : coo_src) {
      if (v == src)
        count++;
    }
    return {PeakStatus::OK(), count};
  }
  std::pair<PeakStatus, size_t> impl_getInDegree(const VertexType &dest) {
    LOG_INFO("Inside COO:getInDegree");
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
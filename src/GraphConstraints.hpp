#pragma once
#include "StorageEngine/DebugUtils.hpp"
#include "StorageEngine/GraphContext.hpp"
#include "StorageEngine/Utils.hpp"

namespace CinderPeak {
template <typename VertexType, typename EdgeType> struct GraphConstraints {
  static PeakStatus checkAddEdgeConstraints(
      const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
      const VertexType &src, const VertexType &dest, const EdgeType &weight) {

    if (!ctx.active_storage->impl_hasVertex(src)) {
      return PeakStatus::VertexNotFound("Source vertex does not exist: " +
                                        vertexStr(src));
    }
    if (!ctx.active_storage->impl_hasVertex(dest)) {
      return PeakStatus::VertexNotFound("Destination vertex does not exist: " +
                                        vertexStr(dest));
    }

    if (src == dest) {
      return PeakStatus::InvalidArgument("Self loops are not allowed: " +
                                         edgeStr(src, dest));
    }

    bool isWeighted = ctx.metadata->isGraphWeighted();
    bool isDirected =
        ctx.create_options->hasOption(GraphCreationOptions::Directed);

    bool exists = false;

    if (isWeighted) {
      exists = ctx.active_storage->impl_doesEdgeExist(src, dest, weight);
      if (!isDirected) {
        exists =
            exists || ctx.active_storage->impl_doesEdgeExist(dest, src, weight);
      }
    } else {
      exists = ctx.active_storage->impl_doesEdgeExist(src, dest);
      if (!isDirected) {
        exists = exists || ctx.active_storage->impl_doesEdgeExist(dest, src);
      }
    }

    if (exists) {
      return PeakStatus::EdgeAlreadyExists("Edge already exists: " +
                                           edgeStr(src, dest));
    }

    return PeakStatus::OK();
  }
  static PeakStatus
  checkRemoveEdge(const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
                  const VertexType &src, const VertexType &dest) {
    if (!ctx.active_storage->impl_hasVertex(src)) {
      return PeakStatus::VertexNotFound("Source vertex does not exist: " +
                                        vertexStr(src));
    }
    if (!ctx.active_storage->impl_hasVertex(dest)) {
      return PeakStatus::VertexNotFound("Destination vertex does not exist: " +
                                        vertexStr(dest));
    }
    if (src == dest) {
      return PeakStatus::InvalidArgument("Self loops are not allowed: " +
                                         edgeStr(src, dest));
    }
    return PeakStatus::OK();
  }
};

} // namespace CinderPeak

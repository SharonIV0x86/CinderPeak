#pragma once
#include "engine/GraphContext.hpp"
#include "storage/utils/Utils.hpp"

namespace CinderPeak {
template <typename VertexType, typename EdgeType> struct GraphConstraints {
  static PeakStatus checkAddEdgeConstraints(
      const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
      const VertexType &src, const VertexType &dest, const EdgeType &weight) {

    if (!ctx.active_storage->impl_hasVertex(src) ||
        !ctx.active_storage->impl_hasVertex(dest)) {
      return PeakStatus::VertexNotFound(
          "Source or destination vertex is missing.");
    }

    if (src == dest) {
      if (!ctx.create_options->hasOption(GraphCreationOptions::SelfLoops)) {
        return PeakStatus::InvalidArgument("Self loops are not allowed");
      }
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
      if (!ctx.create_options->hasOption(GraphCreationOptions::ParallelEdges)) {
        return PeakStatus::EdgeAlreadyExists("Edge Already Exists");
      }
    }

    return PeakStatus::OK();
  }
  static PeakStatus
  checkRemoveEdge(const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
                  const VertexType &src, const VertexType &dest) {
    if (!ctx.active_storage->impl_hasVertex(src) ||
        !ctx.active_storage->impl_hasVertex(dest)) {
      return PeakStatus::VertexNotFound(
          "Source or destination vertex is missing.");
    }
    if (src == dest) {
      if (!ctx.create_options->hasOption(GraphCreationOptions::SelfLoops)) {
        return PeakStatus::InvalidArgument("Self loops are not allowed");
      }
    }
    return PeakStatus::OK();
  }
};

} // namespace CinderPeak

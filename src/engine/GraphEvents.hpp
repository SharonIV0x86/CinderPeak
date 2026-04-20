#pragma once
#include "engine/GraphContext.hpp"
#include "storage/utils/Utils.hpp"

namespace CinderPeak {
template <typename VertexType, typename EdgeType> struct GraphEvents {
  static void
  onEdgeAdded(const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
              const VertexType &src, const VertexType &dest) {

    if (ctx.create_options->hasOption(GraphCreationOptions::Directed) &&
        ctx.active_storage->impl_doesEdgeExist(dest, src)) {
      ctx.metadata->updateParallelEdgeCount(PeakStore::UpdateOp::Add);
    }

    if (src == dest) {
      ctx.metadata->updateSelfLoopCount(PeakStore::UpdateOp::Add);
    }

    ctx.metadata->updateEdgeCount(PeakStore::UpdateOp::Add);
  }
  static void
  onEdgeRemove(const PeakStore::GraphContext<VertexType, EdgeType> &ctx,
               const VertexType &src, const VertexType &dest) {

    if (src == dest) {
      ctx.metadata->updateSelfLoopCount(PeakStore::UpdateOp::Remove);
    }

    ctx.metadata->updateEdgeCount(PeakStore::UpdateOp::Remove);
  }
};
} // namespace CinderPeak
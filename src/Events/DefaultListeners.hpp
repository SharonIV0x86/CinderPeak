#pragma once
#include "PeakStore.hpp"
#include "StorageEngine/GraphContext.hpp"
namespace CinderPeak {
template <typename V, typename E>
void registerMetadataListeners(PeakStore::GraphContext<V, E> &ctx) {
  auto metadata = ctx.metadata;

  ctx.events.edgeAdded.subscribe([metadata](const auto &event) {
    metadata->updateEdgeCount(PeakStore::UpdateOp::Add);
  });

  ctx.events.edgeRemoved.subscribe([metadata](const auto &event) {
    metadata->updateEdgeCount(PeakStore::UpdateOp::Remove);
  });

  // ✅ NEW: vertex event listeners
  ctx.events.vertexAdded.subscribe([metadata](const auto &event) {
    metadata->updateVertexCount(PeakStore::UpdateOp::Add);
  });

  ctx.events.vertexRemoved.subscribe([metadata](const auto &event) {
    metadata->updateVertexCount(PeakStore::UpdateOp::Remove);
  });
}
} // namespace CinderPeak
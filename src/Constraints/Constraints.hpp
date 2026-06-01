#pragma once

#include "Operations/GraphOperations.hpp"
#include "StorageEngine/ErrorCodes.hpp"
namespace CinderPeak {
template <typename V, typename E>
PeakStatus validateAddEdge(AddEdgeOperation<V, E> &op) {

  auto *storage = op.ctx.active_storage.get();

  // Config mismatch: weighted vs unweighted
  bool metadataWeighted = op.ctx.metadata->isGraphWeighted();
  if (op.weighted != metadataWeighted) {
    if (op.weighted && !metadataWeighted) {
      return PeakStatus::InvalidArgument(
          "Cannot add weighted edge to an unweighted graph.");
    }
    return PeakStatus::InvalidArgument(
        "Cannot add unweighted edge to a weighted graph.");
  }

  // Vertex existence checks
  if (!storage->impl_hasVertex(op.src)) {
    return PeakStatus::VertexNotFound("Source vertex does not exist.");
  }
  if (!storage->impl_hasVertex(op.dest)) {
    return PeakStatus::VertexNotFound("Destination vertex does not exist.");
  }

  // Self-loop check
  if (op.src == op.dest) {
    return PeakStatus::InvalidArgument("Self-loops are not allowed.");
  }

  // Duplicate edge check
  bool exists = false;

  if (op.weighted) {
    exists = storage->impl_doesEdgeExist(op.src, op.dest, op.weight);
    if (!op.directed) {
      exists =
          exists || storage->impl_doesEdgeExist(op.dest, op.src, op.weight);
    }
  } else {
    exists = storage->impl_doesEdgeExist(op.src, op.dest);
    if (!op.directed) {
      exists = exists || storage->impl_doesEdgeExist(op.dest, op.src);
    }
  }

  if (exists) {
    return PeakStatus::EdgeAlreadyExists(
        "Edge already exists between these vertices.");
  }

  return PeakStatus::OK();
}
} // namespace CinderPeak
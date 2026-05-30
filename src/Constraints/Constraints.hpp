#pragma once

#include "Operations/GraphOperations.hpp"
#include "StorageEngine/DebugUtils.hpp"
#include "StorageEngine/ErrorCodes.hpp"
namespace CinderPeak {
template <typename V, typename E>
PeakStatus validateAddEdge(AddEdgeOperation<V, E> &op) {

  auto *storage = op.ctx.active_storage.get();
  if (!storage->impl_hasVertex(op.src)) {

    return PeakStatus::VertexNotFound("Source vertex does not exist: " +
                                      vertexStr(op.src));
  }
  if (!storage->impl_hasVertex(op.dest)) {

    return PeakStatus::VertexNotFound("Destination vertex does not exist: " +
                                      vertexStr(op.dest));
  }

  if (op.src == op.dest) {

    return PeakStatus::InvalidArgument("Self loops are not allowed: " +
                                       edgeStr(op.src, op.dest));
  }

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

    return PeakStatus::EdgeAlreadyExists("Edge already exists: " +
                                         edgeStr(op.src, op.dest));
  }

  return PeakStatus::OK();
}
} // namespace CinderPeak
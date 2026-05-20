#pragma once
#include "StorageEngine/GraphContext.hpp"
namespace CinderPeak {

template <typename V, typename E> struct AddEdgeOperation {

  CinderPeak::PeakStore::GraphContext<V, E> &ctx;

  const V &src;
  const V &dest;
  const E &weight;

  bool weighted;
  bool directed;
};
} // namespace CinderPeak
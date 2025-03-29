#pragma once
#include <iostream>
#include <memory>

namespace CinderPeak {
namespace PeakStore {
enum class GraphType : uint8_t {
  Directed = 1 << 0,
  Undirected = 1 << 1,
  Weighted = 1 << 2,
  Unweighted = 1 << 3,
  SelfLoops = 1 << 4,
  ParallelEdges = 1 << 5
};
class GraphInternalMetadata {
public:
  size_t num_vertices;
  size_t num_edges;
};
struct GraphCreationOptions {
  uint8_t options = 0;

  GraphCreationOptions(std::initializer_list<GraphType> opts) {
    for (auto opt : opts)
      options |= static_cast<uint8_t>(opt);
  }

  bool hasOption(GraphType opt) const {
    return options & static_cast<uint8_t>(opt);
  }
};
} // namespace PeakStore

} // namespace CinderPeak

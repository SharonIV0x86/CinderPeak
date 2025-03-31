#pragma once
#include <bitset>
#include <iostream>
#include <memory>
namespace CinderPeak {
class GraphCreationOptions {
public:
  enum GraphType {
    Directed = 0,
    Weighted,
    SelfLoops,
    ParallelEdges,
  };
  GraphCreationOptions(std::initializer_list<GraphType> graph_types) {
    for (auto type : graph_types) {
      options.set(type);
    }
  }

  bool hasOption(GraphType type) const { return options.test(type); }

private:
  std::bitset<8> options;
};

namespace PeakStore {
class GraphInternalMetadata {
public:
  size_t density;
  size_t num_vertices;
  size_t num_edges;
  size_t num_self_loops;
  size_t num_parallel_edges;
  GraphInternalMetadata(){
    num_vertices = 0;
    num_edges = 0;
    density = 0;
    num_self_loops = 0;
    num_parallel_edges = 0;
  }
};
} // namespace PeakStore

} // namespace CinderPeak

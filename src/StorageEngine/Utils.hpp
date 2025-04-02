#pragma once
#include "ErrorCodes.hpp"
#include <bitset>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
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
template <typename T, typename Enable = void> struct VertexHasher;
template <typename T, typename Enable = void> struct EdgeHasher;

template <typename T>
struct VertexHasher<
    T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                        std::is_same_v<T, std::string>>> {
  std::size_t operator()(const T &v) const { return std::hash<T>{}(v); }
};

template <typename T>
struct VertexHasher<T, std::enable_if_t<std::is_class_v<T> &&
                                        !std::is_same_v<T, std::string>>> {
  std::size_t operator()(const T &v) const {
    return std::hash<int>{}(v.__id_) ^
           (std::hash<std::string>{}(v.__v___name) << 1);
  }
};
template <typename T>
struct EdgeHasher<
    T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                        std::is_same_v<T, std::string>>> {
  std::size_t operator()(const T &v) const { return std::hash<T>{}(v); }
};
// pair hasher for future use.
template <typename VertexType, typename EdgeType> struct PairHasher {
  std::size_t operator()(const std::pair<VertexType, EdgeType> &p) const {
    return VertexHasher<VertexType>{}(p.first) ^
           (EdgeHasher<EdgeType>{}(p.second) << 1);
  }
};

namespace PeakStore {
class GraphInternalMetadata {
public:
  size_t density;
  size_t num_vertices;
  size_t num_edges;
  size_t num_self_loops;
  size_t num_parallel_edges;
  GraphInternalMetadata() {
    num_vertices = 0;
    num_edges = 0;
    density = 0;
    num_self_loops = 0;
    num_parallel_edges = 0;
  }
};
} // namespace PeakStore

} // namespace CinderPeak

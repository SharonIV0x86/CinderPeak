#pragma once
#include "CinderExceptions.hpp"
#include "ErrorCodes.hpp"
#include "PeakLogger.hpp"
#include <bitset>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace CinderPeak {

using VertexId = uint64_t;

class GraphCreationOptions {
public:
  enum GraphType { Directed = 0, SelfLoops, ParallelEdges, Undirected };
  GraphCreationOptions(std::initializer_list<GraphType> graph_types) {
    for (auto type : graph_types) {
      options.set(type);
    }
  }
  static GraphCreationOptions getDefaultCreateOptions() {
    const GraphCreationOptions DEFAULT_GRAPH_OPTIONS(
        {GraphCreationOptions::Directed, GraphCreationOptions::SelfLoops});
    return DEFAULT_GRAPH_OPTIONS;
  }

  bool hasOption(GraphType type) const { return options.test(type); }

private:
  std::bitset<8> options;
};

// Forward declaration of hashers
template <typename T, typename Enable = void> struct VertexHasher;
template <typename T, typename Enable = void> struct EdgeHasher;

// Primitive or std::string hashing - uses std::hash
template <typename T>
struct VertexHasher<
    T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                        std::is_same_v<T, std::string>>> {
  std::size_t operator()(const T &v) const noexcept {
    return std::hash<T>{}(v);
  }
};

// Helper to detect __id_ member for user-defined types
template <typename, typename = void> struct has___id : std::false_type {};
template <typename T>
struct has___id<T, std::void_t<decltype(std::declval<T>().__id_)>>
    : std::true_type {};

// Class types (user-defined vertex classes).
// IMPORTANT: Only hash stable identity fields (e.g. __id_). Do NOT rely on
// mutable names. This will fail at compile-time if the user type does not
// provide a stable __id_. This is desirable: it forces the user to expose a
// stable identity for consistent hashing.
template <typename T>
struct VertexHasher<T, std::enable_if_t<std::is_class_v<T> &&
                                        !std::is_same_v<T, std::string>>> {
  static_assert(has___id<T>::value,
                "VertexType must provide a stable member '__id_' for hashing");
  std::size_t operator()(const T &v) const noexcept {
    return std::hash<size_t>{}(v.__id_);
  }
  VertexHasher() = default;
  VertexHasher(const VertexHasher &) = default;
  VertexHasher &operator=(const VertexHasher &) = default;
};

// Edge hasher for primitive types and string
template <typename T>
struct EdgeHasher<
    T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T> ||
                        std::is_same_v<T, std::string>>> {
  std::size_t operator()(const T &v) const noexcept {
    return std::hash<T>{}(v);
  }
};

// Pair hasher for (VertexType, EdgeType) pairs (used rarely)
template <typename VertexType, typename EdgeType> struct PairHasher {
  std::size_t
  operator()(const std::pair<VertexType, EdgeType> &p) const noexcept {
    return VertexHasher<VertexType>{}(p.first) ^
           (EdgeHasher<EdgeType>{}(p.second) << 1);
  }
};

// Random name generator (improved: thread_local RNG to avoid repeated
// expensive reseeding on every call)
inline std::string __generate_vertex_name() {
  thread_local std::mt19937 gen([] {
    std::random_device rd;
    std::seed_seq seq{
        rd(), rd(), rd(),
        static_cast<unsigned>(std::chrono::high_resolution_clock::now()
                                  .time_since_epoch()
                                  .count())};
    return std::mt19937(seq);
  }());

  const std::string charset =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  const size_t name_length = 10;
  std::uniform_int_distribution<> dist(0, static_cast<int>(charset.size() - 1));
  std::stringstream ss;
  for (size_t i = 0; i < name_length; ++i) {
    ss << charset[dist(gen)];
  }
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch())
                      .count();
  static std::atomic<uint64_t> name_counter{0};
  ss << "_" << duration << "_"
     << name_counter.fetch_add(1, std::memory_order_relaxed);
  return ss.str();
}

// A simple vertex class for examples / tests. Keep as user-data only.
// Note: __id_ here is not the engine's VertexId (they are independent).
class CinderVertex {
public:
  size_t __id_;
  static size_t nextId;
  std::string __v___name;

  CinderVertex() {
    __id_ = nextId++;
    __v___name = __generate_vertex_name();
  }
  CinderVertex(std::string vertexName) : __v___name{vertexName} {
    __id_ = nextId++;
  };

  bool operator<(const CinderVertex &other) const {
    return __id_ < other.__id_;
  }
  bool operator==(const CinderVertex &other) const {
    return __id_ == other.__id_;
  }
  bool operator!=(const CinderVertex &other) const {
    return this->__id_ != other.__id_;
  }
  virtual ~CinderVertex() = default;
  const std::string __to_vertex_string() const { return __v___name; }
};

class CinderEdge {
public:
  size_t __id_;
  static size_t nextId;
  std::string __e___name;
  CinderEdge() {
    __id_ = nextId++;
    __e___name = __generate_vertex_name();
  }
  CinderEdge(std::string edge_name) : __e___name{edge_name} {
    __id_ = nextId++;
  };

  bool operator<(const CinderEdge &other) const { return __id_ < other.__id_; }
  bool operator>(const CinderEdge &other) const { return __id_ > other.__id_; }
  bool operator==(const CinderEdge &other) const {
    return __id_ == other.__id_;
  }
  bool operator!=(const CinderEdge &other) const {
    return this->__id_ != other.__id_;
  }
  virtual ~CinderEdge() = default;

  const std::string __to_edge_string() const { return __e___name; }
};

inline size_t CinderVertex::nextId = 1;
inline size_t CinderEdge::nextId = 1;

namespace Exceptions {
inline void handle_exception_map(const PeakStatus &status) {
  switch (static_cast<int>(status.code())) {
  case static_cast<int>(StatusCode::NOT_FOUND):
    LOG_INFO("Resource Not Found");
    break;
  case static_cast<int>(StatusCode::UNIMPLEMENTED):
    LOG_WARNING("Called an Unimplemented method");
    break;
  case static_cast<int>(StatusCode::ALREADY_EXISTS):
    LOG_INFO("Resource Already Exists");
    break;
  case static_cast<int>(StatusCode::VERTEX_ALREADY_EXISTS):
    LOG_INFO("Vertex Already Exists");
    break;
  case static_cast<int>(StatusCode::VERTEX_NOT_FOUND):
    LOG_ERROR("Vertex does not exist");
    break;
  case static_cast<int>(StatusCode::EDGE_ALREADY_EXISTS):
    LOG_INFO("Edge Already Exists");
    break;
  default:
    LOG_CRITICAL("Unhandled Exception Occurred");
    break;
  }
}
} // namespace Exceptions

struct Unweighted {};
inline bool operator==(const Unweighted &, const Unweighted &) noexcept {
  return true;
}

} // namespace CinderPeak

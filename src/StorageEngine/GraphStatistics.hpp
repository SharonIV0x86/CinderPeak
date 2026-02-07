#pragma once
#include <atomic>
#include <bitset>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace CinderPeak {
namespace PeakStore {

enum class UpdateOp : uint8_t { Add, Remove, Clear };

class GraphInternalMetadata {
private:
  size_t num_vertices;
  size_t num_edges;
  size_t num_self_loops;
  size_t num_parallel_edges;
  float density;
  const std::string graph_type;
  bool is_vertex_type_primitive;
  bool is_edge_type_primitive;
  bool is_graph_weighted;
  bool is_graph_unweighted;

  mutable std::shared_mutex _mtx;

public:
  GraphInternalMetadata(const std::string &graphType, bool vertex_tp_p,
                        bool edge_tp_p, bool weighted, bool unweighted)
      : graph_type(graphType), is_vertex_type_primitive(vertex_tp_p),
        is_edge_type_primitive(edge_tp_p) {

    num_vertices = 0;
    num_edges = 0;
    density = 0.0;
    num_self_loops = 0;
    num_parallel_edges = 0;
    is_graph_weighted = weighted;
    is_graph_unweighted = unweighted;
  }

  // Custom copy constructor that doesn't copy the mutex
  GraphInternalMetadata(const GraphInternalMetadata &metadata)
      : graph_type(metadata.graph_type) {
    std::shared_lock<std::shared_mutex> lock(metadata._mtx);
    num_vertices = metadata.num_vertices;
    num_edges = metadata.num_edges;
    num_self_loops = metadata.num_self_loops;
    num_parallel_edges = metadata.num_parallel_edges;
    density = metadata.density;
    is_vertex_type_primitive = metadata.is_vertex_type_primitive;
    is_edge_type_primitive = metadata.is_edge_type_primitive;
    is_graph_weighted = metadata.is_graph_weighted;
    is_graph_unweighted = metadata.is_graph_unweighted;
  }

  // Custom copy assignment operator
  GraphInternalMetadata &operator=(const GraphInternalMetadata &metadata) {
    if (this != &metadata) {
      std::shared_lock<std::shared_mutex> metadata_lock(metadata._mtx);
      std::unique_lock<std::shared_mutex> this_lock(_mtx);
      num_vertices = metadata.num_vertices;
      num_edges = metadata.num_edges;
      num_self_loops = metadata.num_self_loops;
      num_parallel_edges = metadata.num_parallel_edges;
      density = metadata.density;
      is_vertex_type_primitive = metadata.is_vertex_type_primitive;
      is_edge_type_primitive = metadata.is_edge_type_primitive;
      is_graph_weighted = metadata.is_graph_weighted;
      is_graph_unweighted = metadata.is_graph_unweighted;
    }
    return *this;
  }

  GraphInternalMetadata(GraphInternalMetadata &&) = delete;
  GraphInternalMetadata &operator=(GraphInternalMetadata &&) = delete;

  bool isGraphWeighted() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return is_graph_weighted;
  }
  bool isGraphUnweighted() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return is_graph_unweighted;
  }

  size_t numEdges() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return num_edges;
  }
  size_t numVertices() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return num_vertices;
  }
  std::string graphType() {
    std::shared_lock<std::shared_mutex> lock(_mtx);
    return graph_type;
  }

  void updateEdgeCount(const UpdateOp &opt) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (opt == UpdateOp::Add)
      num_edges++;
    else if (opt == UpdateOp::Remove)
      num_edges--;
    else if (opt == UpdateOp::Clear)
      num_edges = 0;
  }

  void updateVertexCount(const UpdateOp &opt) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (opt == UpdateOp::Add)
      num_vertices++;
    else if (opt == UpdateOp::Remove)
      num_vertices--;
    else if (opt == UpdateOp::Clear)
      num_vertices = 0;
  }

  void updateParallelEdgeCount(const UpdateOp &opt) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (opt == UpdateOp::Add)
      num_parallel_edges++;
    else if (opt == UpdateOp::Remove)
      num_parallel_edges--;
    else if (opt == UpdateOp::Clear)
      num_parallel_edges = 0;
  }

  void updateSelfLoopCount(const UpdateOp &opt) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (opt == UpdateOp::Add)
      num_self_loops++;
    else if (opt == UpdateOp::Remove)
      num_self_loops--;
    else if (opt == UpdateOp::Clear)
      num_self_loops = 0;
  }

  void updateDensity(bool directed) {
    std::unique_lock<std::shared_mutex> lock(_mtx);

    if (num_vertices <= 1) {
      density = 0.0f;
    }
    auto max_edges = static_cast<float>(num_vertices * (num_vertices - 1));
    auto directed_density = static_cast<float>(num_edges) / max_edges;
    if (directed)
      density = directed_density;
    else
      density = 2 * directed_density;
  }

  std::string getGraphStatistics(bool directed) {
    updateDensity(directed);

    std::shared_lock<std::shared_mutex> lock(_mtx);

    std::stringstream ss;
    ss << "=== Graph Statistics ===" << std::endl;
    ss << "Vertices: " << num_vertices << std::endl;
    ss << "Edges: " << num_edges << std::endl;
    ss << "Density: " << std::fixed << std::setprecision(2) << density
       << std::endl;
    ss << "Self-loops: " << num_self_loops << std::endl;
    ss << "Parallel edges: " << num_parallel_edges << std::endl;

    return ss.str();
  }
};
} // namespace PeakStore
} // namespace CinderPeak
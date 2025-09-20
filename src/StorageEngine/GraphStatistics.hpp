#pragma once
#include <atomic>
#include <bitset>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace CinderPeak {
namespace PeakStore {
class GraphInternalMetadata {
private:
  mutable std::shared_mutex mutex;

  std::atomic<size_t> num_vertices{0};
  std::atomic<size_t> num_edges{0};
  std::atomic<size_t> num_self_loops{0};
  std::atomic<size_t> num_parallel_edges{0};

  float density;
  const std::string graph_type;
  bool is_vertex_type_primitive;
  bool is_edge_type_primitive;
  bool is_graph_weighted;
  bool is_graph_unweighted;

  const float calculateDensity(bool directed) {
    auto num_vertices = num_vertices_.load(std::memory_order_acquire);
    auto num_edges = num_edges_.load(std::memory_order_acquire);

    if (num_vertices <= 1) {
      return 0.0f;
    }

    const float directed_density =
        static_cast<float>(num_edges) / (num_vertices * (num_vertices - 1));
    if (directed)
      return directed_density;
    return 2.0f * directed_density;
  }

public:
  GraphInternalMetadata(const std::string &graph_type, bool vertex_tp_p,
                        bool edge_tp_p, bool weighted, bool unweighted)
      : graph_type(graph_type), is_vertex_type_primitive(vertex_tp_p),
        is_edge_type_primitive(edge_tp_p) {
    density = 0.0;
    is_graph_weighted = weighted;
    is_graph_unweighted = unweighted;
  }

  const bool isGraphWeighted() { return is_graph_weighted; }
  const bool isGraphUnweighted() { return is_graph_unweighted; }

  size_t numEdges() { return num_edges.load(std::memory_order_acquire); }
  size_t numVertices() { return num_vertices.load(std::memory_order_acquire); }
  const std::string graphType() { return graph_type; }

  void updateEdgeCount(std::string opt) {
    if (opt == "add")
      num_edges.fetch_add(1, std::memory_order_acq_rel);
    if (opt == "remove")
      num_edges.fetch_sub(1, std::memory_order_acq_rel);
    if (opt == "clear")
      num_edges.store(0, std::memory_order_release)
  }

  void updateVertexCount(std::string opt) {
    if (opt == "add")
      num_vertices.fetch_add(1, std::memory_order_acq_rel);
    if (opt == "remove")
      num_vertices.fetch_sub(1, std::memory_order_acq_rel);
    if (opt == "clear")
      num_vertices.store(0, std::memory_order_release)
  }

  void updateParallelEdgeCount(std::string opt) {
    if (opt == "add")
      num_parallel_edges.fetch_add(1, std::memory_order_acq_rel);
    if (opt == "remove")
      num_parallel_edges.fetch_sub(1, std::memory_order_acq_rel);
    if (opt == "clear")
      num_parallel_edges.store(0, std::memory_order_release)
  }

  void updateSelfLoopCount(std::string opt) {
    if (opt == "add")
      num_self_loops.fetch_add(1, std::memory_order_acq_rel);
    if (opt == "remove")
      num_self_loops.fetch_sub(1, std::memory_order_acq_rel);
    if (opt == "clear")
      num_self_loops.store(0, std::memory_order_release)
  }

  const std::string getGraphStatistics(bool directed) {
    std::unique_lock<std::shared_mutex> lock(mutex);

    auto num_vertices = num_vertices.load(std::memory_order_acquire);
    auto num_edges = num_edges.load(std::memory_order_acquire);
    auto num_self_loops = num_self_loops.load(std::memory_order_acquire);
    auto num_parallel_edges =
        num_parallel_edges.load(std::memory_order_acquire);
    auto density = calculateDensity(directed);

    lock.unlock();

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
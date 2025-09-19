#pragma once
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
#include <utility>
#include <vector>

namespace CinderPeak {
namespace PeakStore {
class GraphInternalMetadata {
public:
  float density;
  size_t num_vertices;
  size_t num_edges;
  size_t num_self_loops;
  size_t num_parallel_edges;
  const std::string graph_type;
  bool is_vertex_type_primitive;
  bool is_edge_type_primitive;
  bool is_graph_weighted;
  bool is_graph_unweighted;

  GraphInternalMetadata(const std::string &graph_type, bool vertex_tp_p,
                        bool edge_tp_p, bool weighted, bool unweighted)
      : graph_type(graph_type), is_vertex_type_primitive(vertex_tp_p),
        is_edge_type_primitive(edge_tp_p) {
    num_vertices = 0;
    num_edges = 0;
    density = 0.0;
    num_self_loops = 0;
    num_parallel_edges = 0;
    is_graph_weighted = weighted;
    is_graph_unweighted = unweighted;
  }
  const bool isGraphWeighted() { return is_graph_weighted; }
  const bool isGraphUnweighted() { return is_graph_unweighted; }

  size_t numEdges() { return num_edges; }
  size_t numVertices() { return num_vertices; }

  void updateEdgeCount(std::string opt) {
    if (opt == "add")
      num_edges++;
    if (opt == "remove")
      num_edges--;
    if (opt == "clear")
      num_edges = 0;
  }

  void updateVertexCount(std::string opt) {
    if (opt == "add")
      num_vertices++;
    if (opt == "remove")
      num_vertices--;
    if (opt == "clear")
      num_vertices = 0;
  }

  void updateParallelEdgeCount(std::string opt) {
    if (opt == "add")
      num_parallel_edges++;
    if (opt == "remove")
      num_parallel_edges--;
    if (opt == "clear")
      num_parallel_edges = 0;
  }

  void updateSelfLoopCount(std::string opt) {
    if (opt == "add")
      num_self_loops++;
    if (opt == "remove")
      num_self_loops--;
    if (opt == "clear")
      num_self_loops = 0;
  }

  std::stringstream getGraphStatistics(bool directed) {
    std::stringstream ss;

    if (num_vertices > 1) {
      float directed_density =
          (float)num_edges / (num_vertices * (num_vertices - 1));
      if (directed)
        density = directed_density;
      else
        density = 2 * directed_density;
    }
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
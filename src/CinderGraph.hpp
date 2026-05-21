#pragma once
#include "Algorithms/CinderPeakAlgorithms.hpp"
#include "Concepts.hpp"
#include "PeakStore.hpp"
#include "StorageEngine/DebugUtils.hpp"
#include "StorageEngine/GraphStatistics.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <optional>
#include <tuple>
#include <utility>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}

/**
 * @brief Primary graph container for the CinderPeak graph library.
 *
 * CinderGraph provides a flexible graph abstraction supporting both
 * weighted and unweighted graphs with customizable vertex and edge types.
 *
 * The graph delegates storage and orchestration responsibilities to the
 * underlying PeakStore backend while exposing a simplified user-facing API.
 *
 * Supported features include:
 * - Directed and undirected graphs
 * - Weighted and unweighted edges
 * - Graph traversal algorithms
 * - Graph statistics and visualization
 * - Configurable logging and exception handling
 *
 * @tparam VertexType Type used to represent graph vertices.
 * @tparam EdgeType Type used to represent graph edges or edge weights.
 *
 * @note Primitive and custom user-defined types are supported.
 */
template <typename VertexType, typename EdgeType> class CinderGraph;
template <typename VertexType, typename EdgeType> class CinderGraphRowProxy {
  CinderGraph<VertexType, EdgeType> &graph;
  VertexType src;

public:
  CinderGraphRowProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s)
      : graph(g), src(s) {}

  EdgeType operator[](const VertexType &dest) const {
    auto optWeight = graph.getEdge(src, dest);
    if (!optWeight.has_value()) {
      throw std::runtime_error("Edge not found: " + edgeStr(src, dest));
    }
    return *optWeight;
  }

  CinderGraphRowProxy &operator=(const EdgeType &newWeight) = delete;

  CinderGraphRowProxy &operator()(const VertexType &dest,
                                  const EdgeType &weight) {
    graph.addEdge(src, dest, weight);
    return *this;
  }

  struct EdgeAssignProxy {
    CinderGraph<VertexType, EdgeType> &graph;
    VertexType src, dest;

    EdgeAssignProxy(CinderGraph<VertexType, EdgeType> &g, const VertexType &s,
                    const VertexType &d)
        : graph(g), src(s), dest(d) {}

    EdgeAssignProxy &operator=(const EdgeType &weight) {
      graph.addEdge(src, dest, weight);
      return *this;
    }

    operator EdgeType() const {
      auto optWeight = graph.getEdge(src, dest);
      return optWeight ? *optWeight : EdgeType{};
    }
  };

  EdgeAssignProxy operator[](const VertexType &dest) {
    return EdgeAssignProxy(graph, src, dest);
  }
};

/**
 * @brief User-facing graph implementation built on top of PeakStore.
 *
 * This class exposes APIs for graph creation, mutation, traversal,
 * querying, visualization, and runtime configuration.
 *
 * Internally, storage operations are delegated to the configured
 * storage backend through PeakStore.
 *
 * @tparam VertexType Vertex representation type.
 * @tparam EdgeType Edge or edge-weight representation type.
 */
template <typename VertexType, typename EdgeType> class CinderGraph {
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

  using EdgeKey = std::pair<VertexType, VertexType>;
  using WeightedEdgeKey = std::tuple<VertexType, VertexType, EdgeType>;

  // Insert / result types (Boost-like)
  using VertexAddResult = std::pair<VertexType, bool>;
  using UnweightedEdgeAddResult = std::pair<EdgeKey, bool>;
  using WeightedEdgeAddResult = std::pair<WeightedEdgeKey, bool>;

  // UpdateEdgeResult: {previousWeight, updatedFlag}
  // (previousWeight is EdgeType{} when edge missing or unknown)
  using UpdateEdgeResult = std::pair<EdgeType, bool>;

  using RemoveEdgeResult = std::pair<std::optional<EdgeType>, bool>;
  using NeighborListResult = std::vector<std::pair<VertexType, EdgeType>>;

public:
  /**
   * @brief Constructs a graph instance with the provided configuration.
   *
   * Initializes graph metadata and configures the underlying storage backend
   * using the supplied graph creation options.
   *
   * @param options Graph configuration options controlling graph behavior.
   *
   * @note Uses default graph creation options when no configuration is
   * provided.
   */
  CinderGraph(const GraphCreationOptions &options =
                  GraphCreationOptions::getDefaultCreateOptions()) {
    PeakStore::GraphInternalMetadata metadata(
        "cinder_graph", Traits::isTypePrimitive<VertexType>(),
        Traits::isTypePrimitive<EdgeType>(),
        Traits::isGraphWeighted<EdgeType>(),
        !Traits::isGraphWeighted<EdgeType>());

    peak_store = std::make_unique<PeakStore::PeakStore<VertexType, EdgeType>>(
        metadata, options);
  }

  /**
   * @brief Adds a vertex to the graph.
   *
   * Inserts the provided vertex into the active graph storage backend.
   *
   * @param v Vertex instance to insert.
   *
   * @return Pair containing:
   * - inserted vertex
   * - insertion success status
   *
   * @complexity
   * Average: O(1)
   *
   * @throws Exception propagated through the configured exception handler.
   */
  VertexAddResult addVertex(const VertexType &v) {
    peak_store->log(LogLevel::INFO, "API: Entering addVertex");
    auto resp = peak_store->addVertex(v);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addVertex");
      Exceptions::handle_exception_map(resp);
      // If exceptions are disabled, handle_exception_map returns -> fallthrough
      return {v, false};
    }
    peak_store->log(LogLevel::INFO, "API: addVertex completed successfully");
    return {v, true};
  }

  /**
   * @brief Removes a vertex and its associated edges from the graph.
   *
   * @param v Vertex to remove.
   *
   * @return true if removal succeeds.
   * @return false otherwise.
   *
   * @complexity
   * Depends on graph storage backend and edge connectivity.
   */
  bool removeVertex(const VertexType &v) {
    peak_store->log(LogLevel::INFO, "API: Entering removeVertex");
    auto resp = peak_store->removeVertex(v);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in removeVertex");
      Exceptions::handle_exception_map(resp);
      return false;
    }
    peak_store->log(LogLevel::INFO, "API: removeVertex completed successfully");
    return true;
  }

  /**
   * @brief Removes an edge between two vertices.
   *
   * Removes the connection between the specified source and destination
   * vertices from the graph.
   *
   * @param src Source vertex.
   * @param dest Destination vertex.
   *
   * @return Pair containing:
   * - removed edge value (if available)
   * - operation success status
   *
   * @complexity
   * Depends on storage backend implementation.
   */
  RemoveEdgeResult removeEdge(const VertexType &src, const VertexType &dest) {
    peak_store->log(LogLevel::INFO, "API: Entering removeEdge");
    auto [data, status] = peak_store->removeEdge(src, dest);
    if (!status.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in removeEdge");
      Exceptions::handle_exception_map(status);
      return {std::nullopt, false};
    }
    peak_store->log(LogLevel::INFO, "API: removeEdge completed successfully");
    return {std::make_optional(data), true};
  }

  /**
   * @brief Removes all vertices from the graph.
   *
   * Clears the entire vertex set along with all associated edges
   * and graph connectivity information.
   *
   * @note This operation resets graph topology state.
   */
  void clearVertices() {
    peak_store->log(LogLevel::INFO, "API: Entering clearVertices");
    auto resp = peak_store->clearVertices();
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in clearVertices");
      Exceptions::handle_exception_map(resp);
      return;
    }
    peak_store->log(LogLevel::INFO,
                    "API: clearVertices completed successfully");
  }

  /**
   * @brief Removes all edges from the graph.
   *
   * Clears all graph connections while preserving existing vertices.
   *
   * @note Vertex storage remains intact after this operation.
   */
  void clearEdges() {
    peak_store->log(LogLevel::INFO, "API: Entering clearEdges");
    auto resp = peak_store->clearEdges();
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in clearEdges");
      Exceptions::handle_exception_map(resp);
      return;
    }
    peak_store->log(LogLevel::INFO, "API: clearEdges completed successfully");
  }

  /**
   * @brief Checks whether a vertex exists in the graph.
   *
   * @param v Vertex to query.
   *
   * @return true if the vertex exists.
   * @return false otherwise.
   *
   * @complexity
   * Average: O(1)
   */
  bool hasVertex(const VertexType &v) { return peak_store->hasVertex(v); }
  template <typename E = EdgeType>

  /**
   * @brief Adds an unweighted edge between two vertices.
   *
   * Creates a connection between the source and destination vertices
   * in an unweighted graph configuration.
   *
   * @param src Source vertex.
   * @param dest Destination vertex.
   *
   * @return Pair containing:
   * - inserted edge key
   * - insertion success status
   *
   * @complexity
   * Average: O(1)
   *
   * @note Available only for unweighted graph configurations.
   */
  auto addEdge(const VertexType &src, const VertexType &dest)
      -> std::enable_if_t<CinderPeak::Traits::is_unweighted_v<E>,
                          UnweightedEdgeAddResult> {
    peak_store->log(LogLevel::INFO, "API: Entering addEdge (unweighted)");
    auto resp = peak_store->addEdge(src, dest);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addEdge (unweighted)");
      Exceptions::handle_exception_map(resp);
      return {{src, dest}, false};
    }
    peak_store->log(LogLevel::INFO,
                    "API: addEdge (unweighted) completed successfully");
    return {{src, dest}, true};
  }

  template <typename E = EdgeType>

  /**
   * @brief Adds a weighted edge between two vertices.
   *
   * Creates a weighted connection between the source and destination
   * vertices using the provided edge weight.
   *
   * @param src Source vertex.
   * @param dest Destination vertex.
   * @param weight Edge weight or edge payload.
   *
   * @return Pair containing:
   * - inserted weighted edge key
   * - insertion success status
   *
   * @complexity
   * Average: O(1)
   *
   * @note Available only for weighted graph configurations.
   */
  auto addEdge(const VertexType &src, const VertexType &dest,
               const EdgeType &weight)
      -> std::enable_if_t<!CinderPeak::Traits::is_unweighted_v<E>,
                          WeightedEdgeAddResult> {
    peak_store->log(LogLevel::INFO, "API: Entering addEdge (weighted)");
    auto resp = peak_store->addEdge(src, dest, weight);
    if (!resp.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in addEdge (weighted)");
      Exceptions::handle_exception_map(resp);
      return {{src, dest, weight}, false};
    }
    peak_store->log(LogLevel::INFO,
                    "API: addEdge (weighted) completed successfully");
    return {{src, dest, weight}, true};
  }

  template <typename E = EdgeType>

  /**
   * @brief Updates the weight or payload of an existing edge.
   *
   * Replaces the current edge weight with the provided value.
   *
   * @param src Source vertex.
   * @param dest Destination vertex.
   * @param newWeight Updated edge weight or payload.
   *
   * @return Pair containing:
   * - previous edge weight
   * - update success status
   *
   * @note Available only for weighted graph configurations.
   */
  auto updateEdge(const VertexType &src, const VertexType &dest,
                  const EdgeType &newWeight)
      -> std::enable_if_t<CinderPeak::Traits::is_weighted_v<E>,
                          UpdateEdgeResult> {
    peak_store->log(LogLevel::INFO, "API: Entering updateEdge");
    auto [status, updatedEdge] = peak_store->updateEdge(src, dest, newWeight);
    if (!status.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in updateEdge");
      Exceptions::handle_exception_map(status);
      return {newWeight, false};
    }
    peak_store->log(LogLevel::INFO, "API: updateEdge completed successfully");
    return {newWeight, true};
  }

  /**
   * @brief Retrieves the edge value between two vertices.
   *
   * @param src Source vertex.
   * @param dest Destination vertex.
   *
   * @return Optional edge value if the edge exists.
   *
   * @complexity
   * Depends on storage backend implementation.
   */
  std::optional<EdgeType> getEdge(const VertexType &src,
                                  const VertexType &dest) {
    auto [data, status] = peak_store->getEdge(src, dest);
    if (!status.isOK()) {
      Exceptions::handle_exception_map(status);
      return std::nullopt;
    }
    return data;
  }

  /**
   * @brief Retrieves all neighbors of a vertex.
   *
   * Returns all adjacent vertices and their corresponding edge values
   * for the specified vertex.
   *
   * @param v Vertex whose neighbors are to be retrieved.
   *
   * @return Vector of {neighbor, edge} pairs for all neighbors of v.
   *         Returns an empty vector if the vertex does not exist or has no
   *         outgoing edges.
   *
   * @complexity
   * O(deg(v)) — proportional to the out-degree of the vertex.
   *
   * @throws Exception propagated through the configured exception handler.
   */
  NeighborListResult getNeighbors(const VertexType &v) const {
    peak_store->log(LogLevel::INFO,
                    "API: Entering getNeighbors for " + vertexStr(v));
    auto [neighbors, status] = peak_store->getNeighbors(v);
    if (!status.isOK()) {
      peak_store->log(LogLevel::WARNING, "API: Error in getNeighbors");
      Exceptions::handle_exception_map(status);
      return {};
    }
    peak_store->log(LogLevel::INFO, "API: getNeighbors completed successfully");
    return neighbors;
  }

  /**
   * @brief Performs Breadth-First Search traversal.
   *
   * Executes BFS traversal starting from the specified source vertex.
   *
   * @param src Starting vertex for traversal.
   *
   * @return BFS traversal result containing visited vertices
   *         and traversal metadata.
   *
   * @throws Exception propagated through the configured exception handler.
   */
  Algorithms::BFSResult<VertexType> bfs(const VertexType &src) {
    return peak_store->bfs(src);
  }

  Algorithms::BFSResult<VertexType>
  bfs(const VertexType &src,
      const std::shared_ptr<
          const Algorithms::TraversalSnapshot<VertexType, EdgeType>>
          &snapshot) {
    return peak_store->bfs(src, snapshot);
  }

  std::shared_ptr<const Algorithms::TraversalSnapshot<VertexType, EdgeType>>
  createTraversalSnapshot(
      Algorithms::TraversalSnapshotBackend backend =
          Algorithms::TraversalSnapshotBackend::Adjacency) const {
    return peak_store->createTraversalSnapshot(backend);
  }

  Algorithms::DFSResult<VertexType> dfs(const VertexType &src) {
    return peak_store->dfs(src);
  }

  Algorithms::DFSResult<VertexType>
  dfs(const VertexType &src,
      const std::shared_ptr<
          const Algorithms::TraversalSnapshot<VertexType, EdgeType>>
          &snapshot) {
    return peak_store->dfs(src, snapshot);
  }

  Algorithms::TopologicalSortResult<VertexType>
  topologicalSort(const std::shared_ptr<const Algorithms::TraversalSnapshot<
                      VertexType, EdgeType>> &snapshot) const {
    return peak_store->topologicalSort(snapshot);
  }

  template <typename V = VertexType, typename E = EdgeType>

  /**
   * @brief Exports the graph in Graphviz DOT format.
   *
   * Generates a DOT representation of the graph that can be
   * visualized using Graphviz-compatible tools.
   *
   * @param filename Output DOT file path.
   *
   * @note Available for primitive vertex and edge types.
   */
  auto toDot(const std::string &filename)
      -> std::enable_if_t<Traits::isTypePrimitive<V>() &&
                          (Traits::isTypePrimitive<E>() ||
                           Traits::is_unweighted_v<E>)> {
    peak_store->toDot(filename);
  }

  /**
   * @brief Retrieves graph statistics and metadata.
   *
   * Provides runtime information about graph structure,
   * storage characteristics, and graph state.
   *
   * @return String representation of graph statistics and metrics.
   */
  std::string getGraphStatistics() { return peak_store->getGraphStatistics(); }
  size_t numEdges() const { return peak_store->numEdges(); }
  size_t numVertices() const { return peak_store->numVertices(); }
  void setConsoleLogging(bool toggle) { peak_store->setConsoleLogging(toggle); }
  void setThrowExceptions(bool toggle) {
    peak_store->setThrowExceptions(toggle);
  }
  void setFileLogging(const std::string &path) {
    peak_store->setFileLogging(path);
  }
  void unsetFileLogging() { peak_store->unsetFileLogging(); }

  CinderGraphRowProxy<VertexType, EdgeType> operator[](const VertexType &v) {
    return CinderGraphRowProxy<VertexType, EdgeType>(*this, v);
  }
  const CinderGraphRowProxy<VertexType, EdgeType>
  operator[](const VertexType &v) const {
    return CinderGraphRowProxy<VertexType, EdgeType>(
        const_cast<CinderGraph<VertexType, EdgeType> &>(*this), v);
  }

  // Set graph name - user-facing API
  bool setGraphName(const std::string &name) {
    peak_store->log(LogLevel::INFO, "API: Setting graph name");
    bool result = peak_store->setGraphName(name);
    if (!result) {
      peak_store->log(LogLevel::WARNING, "API: Invalid graph name provided");
    } else {
      peak_store->log(LogLevel::INFO, "API: Graph name set successfully");
    }
    return result;
  }

  // Get graph name - user-facing API
  std::string getGraphName() {
    peak_store->log(LogLevel::INFO, "API: Entering getGraphName");
    std::string name = peak_store->getGraphName();
    peak_store->log(LogLevel::INFO, "API: getGraphName completed successfully");
    return name;
  }
};

} // namespace CinderPeak
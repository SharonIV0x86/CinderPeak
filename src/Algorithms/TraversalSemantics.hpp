#pragma once

/*
 * CinderPeak Traversal Architecture — Semantics (issue completion)
 *
 * Snapshot immutability
 *   TraversalSnapshot captures vertex-keyed adjacency at creation time.
 *   After construction, neighbor lists and membership are fixed. Live graph
 *   mutations (add/remove vertex/edge) never retroactively change a snapshot.
 *
 * Snapshot ownership
 *   PeakStore::createTraversalSnapshot() returns shared_ptr<const Snapshot>.
 *   Callers may retain snapshots independently of PeakStore. No invalidation
 *   or versioning is performed; snapshots are lightweight value-like views.
 *
 * Traversal consistency (live / callback path)
 *   bfs(src) reads the active storage through PeakStore-injected callbacks.
 *   Order follows backend neighbor ordering at query time. AdjacencyList
 *   preserves insertion order; HybridCSR_COO sorts neighbors by internal index
 *   in impl_getNeighbors. Repeated runs on an unchanged graph are stable.
 *
 * Traversal consistency (snapshot path)
 *   bfs(src, snapshot) reads only the frozen snapshot. Neighbor lists are
 *   stabilized at snapshot construction (deterministic per-vertex order by
 *   vertex identity). Repeated runs on the same snapshot yield identical order.
 *
 * Backend parity (snapshot capture)
 *   Adjacency and Hybrid captures represent the same logical graph when hybrid
 *   is populated from equivalent connectivity. Both paths materialize through
 *   TraversalSnapshot, which applies the same neighbor-order normalization.
 *   Hybrid TraversalSnapshotBackend falls back to adjacency when hybrid is
 *   empty (no automatic rebuild/sync).
 *
 * Fallback semantics
 *   Live traversal: hybrid-first getNeighbors/hasVertex, adjacency fallback.
 *   Snapshot traversal: hybrid capture with adjacency fallback if empty.
 *
 * Determinism limits
 *   Snapshot traversals are deterministic for a given snapshot instance.
 *   Live traversals may change order after mutations that reorder adjacency
 *   lists. No global vertex sorting is applied during traversal (only local
 *   per-vertex neighbor stabilization in snapshots).
 *
 * DFS / topological sort
 *   Use TraversalAdjacencyAccess + TraversalExecution (runDfs,
 * runTopologicalSort). Topological sort requires a TraversalSnapshot and
 * reports GRAPH_CYCLE_DETECTED when the graph is not a DAG.
 */

namespace CinderPeak {
namespace Algorithms {

/// How neighbor lists are ordered before / during traversal execution.
enum class TraversalNeighborOrder {
  /// Use neighbor order supplied by the backend or snapshot as-is.
  PreserveBackendOrder,
  /// Sort neighbors by stable vertex identity (see TraversalExecution.hpp).
  DeterministicByVertex
};

/// Whether traversal reads live storage callbacks or a frozen snapshot.
enum class TraversalExecutionMode { LiveCallbacks, ImmutableSnapshot };

} // namespace Algorithms
} // namespace CinderPeak

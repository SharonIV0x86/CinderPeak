# CinderPeak — Architecture Documentation

This document explains the internal design, data flow, and component responsibilities of the CinderPeak graph library. It is intended for contributors and advanced users who want to understand how the library works under the hood.

---

## 1. High-Level Design

CinderPeak follows the **Separation of Concerns** principle and is organized into three distinct architectural layers. Each layer has a clearly defined responsibility and communicates only with adjacent layers.

```
┌─────────────────────────────────────────────────────┐
│             Layer 0 — User Application              │
│          (code written by library users)            │
└────────────────────────┬────────────────────────────┘
                         │  calls API
┌────────────────────────▼────────────────────────────┐
│        Layer 1 — CinderGraph (User API / Facade)    │
│   Exposes: addVertex, addEdge, bfs, toDot, ...      │
└────────────────────────┬────────────────────────────┘
                         │  delegates to
┌────────────────────────▼────────────────────────────┐
│     Layer 2 — PeakStore (Storage Orchestrator)      │
│   Manages: context, backends, events, constraints   │
└────────────────────────┬────────────────────────────┘
                         │  calls impl_* methods on
┌────────────────────────▼────────────────────────────┐
│     Layer 3 — Storage Backends (Concrete Impls)     │
│   AdjacencyList  |  HybridCSR_COO                   │
└─────────────────────────────────────────────────────┘
```

**Key rules:**
- Layer 1 cannot directly talk to Layer 3 — it must go through Layer 2.
- Layer 3 knows nothing about the API contract — it only implements storage operations.
- Communication between layers is bidirectional (return values flow back up).

---

## 2. Layer Details

### 2.1 Layer 1 — CinderGraph (User API / Facade)

**File:** `src/CinderGraph.hpp`

`CinderGraph<VertexType, EdgeType>` is the primary class that users interact with. It follows the **Facade Pattern** — hiding all internal complexity and exposing a clean, minimal public API.

**What it does:**
- Accepts user calls like `addVertex()`, `addEdge()`, `getNeighbors()`, `bfs()`
- Validates arguments and logs actions via the runtime logger
- Delegates the actual work to `PeakStore` (Layer 2)
- Returns structured results to the user (e.g., `std::pair`, `std::optional`)

**Key type aliases (internal):**

| Type Alias | Definition | Description |
|:-----------|:-----------|:------------|
| `VertexAddResult` | `pair<VertexType, bool>` | Returned by `addVertex` |
| `UnweightedEdgeAddResult` | `pair<pair<V,V>, bool>` | Returned by unweighted `addEdge` |
| `WeightedEdgeAddResult` | `pair<tuple<V,V,E>, bool>` | Returned by weighted `addEdge` |
| `UpdateEdgeResult` | `pair<EdgeType, bool>` | Returned by `updateEdge` |
| `RemoveEdgeResult` | `pair<optional<EdgeType>, bool>` | Returned by `removeEdge` |
| `NeighborListResult` | `vector<pair<VertexType, EdgeType>>` | Returned by `getNeighbors` |

**Example flow — `graph.addEdge(1, 2, 10)`:**
1. `CinderGraph::addEdge` is called (Layer 1)
2. Logs `"API: Entering addEdge (weighted)"`
3. Calls `peak_store->addEdge(1, 2, 10)` (Layer 2)
4. Layer 2 checks constraints, delegates to active backend
5. Returns `PeakStatus::OK()` or an error back to Layer 1
6. Layer 1 packages the result as `WeightedEdgeAddResult` and returns to user

**Operator support:**

`CinderGraph` also supports matrix-style access via `CinderGraphRowProxy`:
```cpp
// Read edge weight
int w = graph[1][2];

// Write edge weight (via proxy's operator())
graph[1](2, 99);   // sets edge (1->2) weight to 99
```

---

### 2.2 Layer 2 — PeakStore (Storage Orchestrator)

**File:** `src/PeakStore.hpp`

`PeakStore<VertexType, EdgeType>` is the **heart** of CinderPeak. It acts as a mediator between the API layer and the concrete storage backends. It follows the **Mediator Pattern** and uses **Dependency Injection** through a shared `GraphContext`.

**What it manages:**

| Component | Type | Purpose |
|:----------|:-----|:--------|
| `ctx` | `shared_ptr<GraphContext>` | Shared state accessible to all sub-components |
| `ctx->active_storage` | `StorageInterface*` | Currently active storage backend |
| `ctx->adjacency_storage` | `AdjacencyList*` | Always-present adjacency list |
| `ctx->hybrid_storage` | `HybridCSR_COO*` | Optimized traversal structure |
| `ctx->metadata` | `GraphInternalMetadata*` | Vertex/edge counts, graph config |
| `ctx->create_options` | `GraphCreationOptions*` | Directed/undirected, self-loops etc. |
| `ctx->runtime` | `GraphRuntime*` | Logging and exception configuration |
| `ctx->algorithms` | `CinderPeakAlgorithms*` | Algorithm execution (currently stub) |

**Responsibilities:**

1. **Routing operations** — determines whether to call weighted or unweighted variants on backends
2. **Directionality handling** — for undirected graphs, automatically adds the reverse edge
3. **Event dispatching** — fires `GraphEvents::onEdgeAdded`, `onEdgeRemove` after mutations
4. **Metadata tracking** — increments/decrements vertex and edge counts
5. **Backend switching** — `active_storage` can point to different backends

**Example flow — `addEdge` in PeakStore:**
```
PeakStore::addEdge(src, dest, weight)
  ├─ Checks if graph is weighted
  ├─ Calls active_storage->impl_addEdge(src, dest, weight)
  ├─ If OK: fires GraphEvents::onEdgeAdded(*ctx, src, dest)
  ├─ If undirected: also calls impl_addEdge(dest, src, weight)
  └─ Returns PeakStatus
```

---

### 2.3 Layer 3 — Storage Backends (Concrete Implementations)

**Files:** `src/StorageEngine/AdjacencyList.hpp`, `src/StorageEngine/HybridCSR_COO.hpp`

Layer 3 provides concrete storage implementations. All backends inherit from `PeakStorageInterface<VertexType, EdgeType>` and implement the same `impl_*` methods.

#### 3.1 AdjacencyList Backend

**File:** `src/StorageEngine/AdjacencyList.hpp`

The **primary and default** storage backend. It is used for all graph mutations (add/remove vertex/edge).

**Internal data structures:**

| Member | Type | Purpose |
|:-------|:-----|:--------|
| `_adj` | `unordered_map<VertexId, vector<pair<VertexId, EdgeType>>>` | Adjacency list: maps vertex ID → list of (neighbor ID, edge weight) pairs |
| `_vertex_data` | `unordered_map<VertexId, VertexType>` | Maps internal vertex ID → user vertex value |
| `_vertex_lookup` | `unordered_map<VertexType, VertexId>` | Reverse map: user vertex → internal ID |
| `_next_vertex_id` | `atomic<VertexId>` | Auto-incrementing ID counter |
| `_mtx` | `shared_mutex` | Reader-writer lock for thread safety |

**Why a two-map design?**

User-visible vertex types (e.g. `int`, `string`, custom class) are stored in `_vertex_lookup` for quick lookup by value. Internally, all operations use compact `VertexId` (`uint64_t`) values — this keeps the adjacency list efficient and decoupled from user types.

**Thread safety model:**
- Read operations (`hasVertex`, `getNeighbors`, `getEdge`) use `shared_lock` — multiple readers allowed concurrently
- Write operations (`addVertex`, `addEdge`, `removeEdge`) use `unique_lock` — exclusive access
- String construction and logging happens **outside** the lock to avoid blocking critical sections

#### 3.2 HybridCSR_COO Backend

**File:** `src/StorageEngine/HybridCSR_COO.hpp`

A hybrid storage format combining:
- **CSR (Compressed Sparse Row)** — memory-efficient, cache-friendly for traversal
- **COO (Coordinate List)** — flexible edge list format

This backend is designed for **high-performance traversal algorithms** (BFS, DFS, etc.). It is currently initialized but not used as the active mutation backend — the adjacency list serves as the canonical source of truth.

> **Note:** The synchronization strategy between AdjacencyList and HybridCSR_COO is an active architectural discussion. See `src/Algorithms/CinderPeakAlgorithms.hpp` for detailed notes on the design challenges.

#### 3.3 Storage Interface Contract

All backends implement (`PeakStorageInterface` virtual methods):

| Method | Description |
|:-------|:------------|
| `impl_addVertex(v)` | Insert a vertex |
| `impl_addEdge(src, dest, weight)` | Insert an edge |
| `impl_removeVertex(v)` | Remove a vertex and all its edges |
| `impl_removeEdge(src, dest)` | Remove a single edge |
| `impl_updateEdge(src, dest, w)` | Update edge weight |
| `impl_getEdge(src, dest)` | Retrieve edge weight |
| `impl_hasVertex(v)` | Check vertex existence |
| `impl_doesEdgeExist(src, dest)` | Check edge existence |
| `impl_clearVertices()` | Remove all vertices |
| `impl_clearEdges()` | Remove all edges |

**AdjacencyList-only methods** (not in the virtual interface):

| Method | Description |
|:-------|:------------|
| `impl_getNeighbors(v)` | Get all neighbors of a vertex |
| `impl_toDot(isDirected)` | Generate Graphviz DOT output |
| `getInternalAdjacency()` | Access raw adjacency map |

---

## 3. Supporting Components

### 3.1 GraphContext

**File:** `src/StorageEngine/GraphContext.hpp`

A **shared context object** that all internal components share. It acts as the single source of truth for configuration and state. This follows the **Shared State pattern** / Dependency Injection.

```cpp
struct GraphContext {
    shared_ptr<GraphInternalMetadata>   metadata;        // vertex/edge counts, type info
    shared_ptr<GraphCreationOptions>    create_options;  // directed/undirected etc.
    shared_ptr<AdjacencyList<V, E>>     adjacency_storage;
    shared_ptr<HybridCSR_COO<V, E>>     hybrid_storage;
    shared_ptr<StorageInterface<V, E>>  active_storage;  // which backend is active
    shared_ptr<GraphRuntime>            runtime;         // logging + exceptions config
    shared_ptr<CinderPeakAlgorithms<V,E>> algorithms;   // algorithm executor
};
```

---

### 3.2 GraphCreationOptions

**File:** `src/StorageEngine/Utils.hpp`

Configures graph behavior at construction time using a bitmask.

```cpp
enum GraphType { Directed = 0, SelfLoops, Undirected };
```

| Option | Bit | Effect |
|:-------|:----|:-------|
| `Directed` | 0 | Edges have direction (A→B ≠ B→A) |
| `SelfLoops` | 1 | Vertices can connect to themselves |
| `Undirected` | 2 | Edges are bidirectional (A—B = B—A) |

**Default options** (when no options given): `{Directed, SelfLoops}`

---

### 3.3 GraphInternalMetadata

**File:** `src/StorageEngine/GraphStatistics.hpp`

Tracks runtime graph statistics in a thread-safe manner:

| Field | Type | Description |
|:------|:-----|:------------|
| `num_vertices` | `size_t` | Total vertex count |
| `num_edges` | `size_t` | Total edge count |
| `num_self_loops` | `size_t` | Self-loop count |
| `density` | `float` | Edge density (computed on request) |
| `graph_name` | `string` | User-assigned or auto-generated name |
| `is_graph_weighted` | `bool` | True if EdgeType ≠ `Unweighted` |

Counts are updated via `UpdateOp` enum (`Add`, `Remove`, `Clear`). Thread safety uses `shared_mutex`.

---

### 3.4 GraphRuntime

**File:** `src/GraphRuntime.hpp`

Controls runtime behavior — logging and exception policy. Users interact with these settings via the `CinderGraph` public API:

```cpp
graph.setConsoleLogging(true);           // enable/disable console output
graph.setFileLogging("logfile.txt");     // enable file logging
graph.unsetFileLogging();                // disable file logging
graph.setThrowExceptions(true);          // throw exceptions on errors
```

---

### 3.5 PeakStatus & Error Codes

**File:** `src/StorageEngine/ErrorCodes.hpp`

All internal operations return a `PeakStatus` object instead of throwing exceptions by default.

| Status | Meaning |
|:-------|:--------|
| `PeakStatus::OK()` | Operation succeeded |
| `PeakStatus::VertexNotFound()` | Vertex does not exist |
| `PeakStatus::VertexAlreadyExists()` | Duplicate vertex insertion |
| `PeakStatus::EdgeNotFound()` | Edge does not exist |
| `PeakStatus::EdgeAlreadyExists()` | Duplicate edge insertion |
| `PeakStatus::InvalidArgument()` | Invalid input (e.g., bad graph name) |
| `PeakStatus::Unimplemented()` | Feature not yet implemented |

---

### 3.6 Type System & Concepts

**File:** `src/Concepts.hpp`

CinderPeak is fully templated. The `CinderPeak::Traits` namespace provides compile-time type checks:

| Trait | What it checks |
|:------|:--------------|
| `is_unweighted_v<E>` | EdgeType is `Unweighted` sentinel |
| `is_weighted_v<E>` | EdgeType is any type other than `Unweighted` |
| `is_numeric_edge_v<E>` | EdgeType is an arithmetic type |
| `isTypePrimitive<T>()` | T is `int`, `float`, `double`, `string`, etc. |
| `isGraphWeighted<E>()` | Runtime check: is EdgeType weighted? |
| `is_hashable_v<T>` | T can be used with `std::hash` |

**Compile-time enforcement macros:**
```cpp
STATIC_ASSERT_WEIGHTED(E)      // fails if EdgeType is Unweighted
STATIC_ASSERT_UNWEIGHTED(E)    // fails if EdgeType is not Unweighted
STATIC_ASSERT_NUMERIC_EDGE(E)  // fails if EdgeType is not arithmetic
STATIC_ASSERT_COMPARABLE_VERTEX(V) // fails if V doesn't support operator<
```

---

### 3.7 GraphEvents

**File:** `src/GraphEvents.hpp`

Provides hooks that fire after successful graph mutations. Currently used for internal bookkeeping:

| Event | Fires when |
|:------|:-----------|
| `onEdgeAdded(ctx, src, dest)` | After successful edge insertion |
| `onEdgeRemove(ctx, src, dest)` | After successful edge removal |

These update edge counts in `GraphInternalMetadata`.

---

### 3.8 GraphConstraints

**File:** `src/GraphConstraints.hpp`

Validates pre-conditions before mutations:

- `checkAddEdgeConstraints()` — ensures both vertices exist, self-loops are allowed if configured, and the edge doesn't already exist
- `checkRemoveEdge()` — ensures both vertices exist before attempting removal

---

### 3.9 PeakLogger

**File:** `src/PeakLogger.hpp`

A configurable logging system with multiple levels:

| Level | Usage |
|:------|:------|
| `DEBUG` | Low-level internal events (impl_* calls) |
| `INFO` | High-level API events (addVertex succeeded) |
| `WARNING` | Non-fatal issues (vertex already exists) |
| `ERROR` | Failed operations (file write error) |
| `CRITICAL` | System-level critical events |

---

## 4. Data Flow: Complete Example

Let's trace `graph.addEdge(1, 2, 10)` from end to end for a directed, weighted `CinderGraph<int, int>`:

```
User calls: graph.addEdge(1, 2, 10)
│
├─ [Layer 1 - CinderGraph::addEdge]
│   ├─ Logs "API: Entering addEdge (weighted)"
│   └─ Calls peak_store->addEdge(1, 2, 10)
│
├─ [Layer 2 - PeakStore::addEdge]
│   ├─ Reads ctx->metadata->isGraphWeighted() → true
│   ├─ Reads ctx->create_options->hasOption(Directed) → true
│   ├─ Calls ctx->active_storage->impl_addEdge(1, 2, 10)
│   │
│   ├─ [Layer 3 - AdjacencyList::impl_addEdge]
│   │   ├─ Acquires unique_lock on _mtx
│   │   ├─ Looks up vertex 1 → VertexId = 1
│   │   ├─ Looks up vertex 2 → VertexId = 2
│   │   ├─ Appends (destId=2, weight=10) to _adj[1]
│   │   └─ Returns PeakStatus::OK()
│   │
│   ├─ Status is OK → fires GraphEvents::onEdgeAdded(*ctx, 1, 2)
│   │   └─ ctx->metadata->updateEdgeCount(UpdateOp::Add)
│   │
│   ├─ Graph is Directed → skip reverse edge
│   └─ Returns PeakStatus::OK()
│
└─ [Layer 1 - CinderGraph::addEdge]
    ├─ Status is OK → logs "API: addEdge (weighted) completed successfully"
    └─ Returns WeightedEdgeAddResult{{1, 2, 10}, true} to user
```

---

## 5. Algorithms Module (Current Status)

**File:** `src/Algorithms/CinderPeakAlgorithms.hpp`

The algorithms module is currently a **deliberate stub**. This is by design — not a missing feature.

**The core challenge:**

CinderPeak uses two storage backends:
- **AdjacencyList** — canonical source of truth for mutations
- **HybridCSR_COO** — optimized for traversal algorithms

Before algorithms can be properly implemented, these questions must be resolved:

1. **When to synchronize?** After every mutation? On-demand before traversal? Continuously?
2. **Concurrency safety?** Can reads on HybridCSR_COO happen while AdjacencyList is being mutated?
3. **Snapshot semantics?** Should traversals see a point-in-time snapshot of the graph?

The BFS stub currently exists to test the API contract:
```cpp
BFSResult<VertexType> bfs(const VertexType& src) {
    // Currently returns hardcoded placeholder data
    // Real implementation pending architecture finalization
}
```

**Planned algorithm support (future):**
- Breadth-First Search (BFS)
- Depth-First Search (DFS)
- Dijkstra's Shortest Path
- Connected Components
- PageRank
- Centrality metrics

---

## 6. Testing Architecture

CinderPeak uses **Google Test (GTest)** for unit and integration testing.

Tests are organized into **five independent pipeline shards**:

| Shard | Tests | Dependencies |
|:------|:------|:-------------|
| Shard 1 | Adjacency List backend | None |
| Shard 2 | CSR backend | None |
| Shard 3 | COO backend | None |
| Shard 4 | Graph Matrix representation | Shards 1–3 must pass |
| Shard 5 | Graph List representation | Shards 1–3 must pass |

**Execution strategy:**
- Shards 1–3 run in parallel
- Shards 4–5 run only if Shards 1–3 pass
- All shards are managed by the CinderFlow orchestrator

---

## 7. Custom Type Requirements

When using user-defined types as vertex or edge types, the following rules apply:

### Vertex Types
- Must inherit from `CinderVertex` (provides `__id_` for hashing)
- Must provide a copy constructor and `operator==`
- `__id_` is used as the hash key — must be stable across copies

### Edge Types (Weighted)
- Must inherit from `CinderEdge` (provides `__id_` for identity)
- Must be copy-constructible and default-constructible

### Edge Types (Unweighted)
- Use the `Unweighted` sentinel type: `CinderGraph<int, Unweighted>`
- `Unweighted` is a zero-size struct — no overhead

---

## 8. Design Pattern Summary

| Component | Pattern |
|:----------|:--------|
| `CinderGraph` | Facade |
| `PeakStore` | Mediator |
| `GraphContext` | Shared State / Dependency Injection |
| `StorageInterface` + backends | Strategy + Bridge |
| `GraphEvents` | Observer |
| `GraphConstraints` | Guard / Pre-condition Checker |
| `GraphRuntime` | Strategy (configurable behavior) |
| `CinderPeakAlgorithms` | Strategy (pluggable algorithms) |
# CinderPeak Core Architecture Documentation

CinderPeak is a modular, high-performance graph processing library designed to handle complex graph structural topologies. This document breaks down the internal architecture layers, data interaction flows, and the underlying storage engines powering the system.

---

## 1. High-Level Architectural Design

The codebase relies on a strict Separation of Concerns pattern. It decouples high-level user API tasks from physical, hardware-conscious memory layouts to protect performance spaces during extreme traversal exercises.+-------------------------------------------------------+
|                 Client Interface API                  |  <-- CinderGraph Interface
+-------------------------------------------------------+
│
▼
+-------------------------------------------------------+
|               Data Orchestration Layer                |  <-- Optimization Router
+-------------------------------------------------------+
│
┌─────────────────┴─────────────────┐
▼                                   ▼
+───────────────────+               +───────────────────+
|    Matrix COO     |               |    Matrix CSR     |
| (Coordinate List) |               | (Sparse Compact)  |
+───────────────────+               +───────────────────+
### Functional Layer Breakdown

### Layer 1 — User API Layer (Facade)
* **Responsibility:** Manages the entry point interface configurations.
* **Component Context:** Governs graph setup strategies without forcing direct visibility into complex multi-pointer vector sets.
* **Operations:** Controls explicit parameters mapping edge properties, weights, and layout declarations.

### Layer 2 — Core Engine & Storage Routing
* **Responsibility:** Translates front-end node assignments into structured structural topologies.
* **Component Context:** Analyzes edge densities dynamically to choose between high-speed update matrices and dense compact memory paths.

### Layer 3 — Physical Storage Engine (`PeakStore`)
* **Responsibility:** Direct hardware-level indexing allocations.
* **Component Context:** Coordinates localized memory offsets via specialized compact layouts to limit processor caching misses during deep node traversals.

---

## 2. Core Library Data Flow

Understanding how a mutation passes down to disk memory layouts is crucial for open-source contributors:

### The `addEdge` Functional Target Pipeline
1. **Validation Check:** The incoming parameters are validated against known layout dimensions to confirm bounds safety.
2. **Density Inspection:** The internal engine verifies if the structural layout satisfies sparsity thresholds.
3. **Storage Placement:**
   * For highly dynamic, active operations, structural properties write directly into the sequential tracking coordinates list.
   * For static, performance-critical read operations, columns pass through a reduction compiler into linear continuous indices.

---

## 3. Storage Structures Reference (Hybrid Design)

To provide quick element insertions alongside low-latency vertex lookups, the library relies on an explicit dual layout structure:

| Architectural Metric | Coordinate Storage (COO) | Compressed Sparse Row (CSR) |
| :--- | :--- | :--- |
| **Primary Structural Intent** | Rapid structural updates & insertions | Fast neighbor traversal algorithms |
| **Underlying Vector Mechanics** | Independent coordinate array tracking | Compact target rows containing offsets |
| **Insertion Time Cost** | $O(1)$ constant overhead | $O(N)$ due to displacement limits |
| **Adjacency Lookup Cost** | Includes localized linear sweeps | Direct pointer index access |

---

## 4. Documentation Mapping & Dependency Paths
When tracking individual features, follow these cross-references:
* Look into **`hybrid_storage.md`** to configure custom structural allocation thresholds.
* Review **`CinderGraph.md`** to adapt template parameter traits for specialized attributes.
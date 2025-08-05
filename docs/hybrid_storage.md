# Hybrid CSR-COO Storage Format Documentation

## Overview

The Hybrid CSR-COO (Compressed Sparse Row + Coordinate List) storage format is a high-performance, dynamic graph storage mechanism implemented in CinderPeak. This hybrid approach is designed to combine the efficiency of CSR for graph traversal with the flexibility of COO for rapid edge insertions and structural modifications.

It is particularly useful in scenarios that require a balance between frequent graph updates and fast read/query performance, such as dynamic social networks, streaming graph data, and real-time recommendation systems.

---

## Motivation

Graph-based systems often need to support two types of operations efficiently:
- High-speed querying and traversal (e.g., BFS, DFS, neighbor lookups)
- Frequent structural modifications (e.g., vertex or edge insertions)

Traditional formats present trade-offs:

| Format | Strengths | Limitations |
|--------|-----------|-------------|
| CSR (Compressed Sparse Row) | Fast queries, compact memory layout | Expensive updates and insertions |
| COO (Coordinate List) | Easy and fast insertions | Slow queries, poor locality of reference |

The Hybrid CSR-COO format addresses these limitations by using a dual-buffered system:
- COO is used as a temporary buffer for dynamic operations.
- CSR is maintained for efficient access and traversals.
- Periodic and incremental synchronization occurs between the two.

---

## Internal Structure

The internal representation maintains both CSR and COO components as follows:

```cpp
// CSR components (read-efficient structure)
std::vector<size_t> csr_row_offsets;
std::vector<VertexType> csr_col_vals;
std::vector<EdgeType> csr_weights;

// COO components (write-efficient structure)
std::vector<VertexType> coo_src;
std::vector<VertexType> coo_dest;
std::vector<EdgeType> coo_weights;
```
#### Additional helper containers:

- vertex_order: Stores the order of vertex insertions.
- vertex_to_index: Maps each vertex to its index in the CSR layout.
----

## Core Functions
`buildStructures()`
##### This method builds the CSR structure from the COO buffer:
- Counts the outgoing edges for each vertex.
- Sorts the destination vertices in each row.
- Fills csr_row_offsets, csr_col_vals, and csr_weights.
- Clears the COO buffer post-build to free up memory.

*This is typically triggered during the initial graph build or manually if required.*



`incrementalUpdate()`
##### This method performs partial CSR updates when new edges are inserted after the graph has been built. It merges the new COO entries into the existing CSR data:
- Allocates new memory for CSR arrays.
- Merges old and new edge data.
- Sorts neighbors.
- Replaces the old CSR with updated arrays.

*This avoids full reconstruction and allows the graph to evolve efficiently.*

----
## Graph Lifecycle
```
[ Add Vertex / Edge ] 
        ↓
[ Stored in COO ]
        ↓
[ buildStructures() ] ← On initial construction
        ↓
[ CSR is Ready ]
        ↓
[ Add More Edges ]
        ↓
[ incrementalUpdate() ] ← On each insert after build
```
This hybrid lifecycle enables high-throughput updates while maintaining fast traversal performance.

-----

### Example Usage
#### Populating from an Adjacency List
```
HybridCSR_COO<int, float> graph;

std::unordered_map<int, std::vector<std::pair<int, float>>> adjList = {
    {0, {{1, 2.5}, {2, 3.1}}},
    {1, {{3, 1.0}}}
};

graph.populateFromAdjList(adjList);
```
#### Adding Vertices and Edges
```
graph.impl_addVertex(4);
graph.impl_addEdge(1, 4, 4.2);  // COO buffer updated
```
#### Querying an Edge
```
auto [weight, status] = graph.impl_getEdge(0, 2);
if (status.isOK()) {
    std::cout << "Edge weight: " << weight << "\n";
}
```

-----
## Text-Based Example of COO to CSR Conversion
#### Before Build (COO Data):
```
coo_src     = [0, 0, 1]
coo_dest    = [1, 2, 3]
coo_weights = [2.5, 3.1, 1.0]
```

#### After Build (CSR Representation):
```
csr_row_offsets = [0, 2, 3]
csr_col_vals    = [1, 2, 3]
csr_weights     = [2.5, 3.1, 1.0]

```
- `csr_row_offsets[i]` gives the start index of neighbors for vertex `i`.
- The neighbors are stored in `csr_col_vals` and `csr_weights`.

----

## Debug Utility
##### A diagnostic function exc() is available to print the current CSR representation of the graph:
```
graph.exc();

```

##### Sample Output:
```
0 -> (1, 2.5) (2, 3.1)
1 -> (3, 1.0)

```
This function aids in quickly verifying internal correctness during development.

----
### Key Advantages
- **Traversal Efficiency**: Maintains sorted adjacency lists via CSR, enabling fast traversals and queries.
- **Dynamic Updates**: Accepts real-time edge insertions without immediate rebuilds.
- **Incremental Merging**: Only updates the affected parts of the structure, reducing overhead.
- **Optimized Memory Use**: Uses `shrink_to_fit()` and pre-allocated buffers to reduce fragmentation.

----
### Summary
The Hybrid CSR-COO format in CinderPeak offers a balanced, practical, and efficient approach to handling dynamic graphs. It preserves the structural integrity and performance benefits of CSR, while embracing the flexibility required for real-world, mutable graph applications through COO.
This format is ideal for systems that:
- Require high-performance querying.
- Support real-time updates.
- Deal with evolving graph topologies.

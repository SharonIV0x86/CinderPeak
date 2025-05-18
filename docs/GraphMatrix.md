# ``GraphMatrix`` API - CinderPeak


The GraphMatrix class in the CinderPeak graph library provides an efficient matrix-based representation for graphs with optional support for weighted/unweighted, directed/undirected, and self-loops.


This document outlines:

- Class design
- API methods
- Supported graph configurations
- Example usage

## Class Template
```cpp
template <typename VertexType, typename EdgeType>
class GraphMatrix;
```
For primitives the simple primitive types must be placed as template arguments.
For ``class`` and ``struct`` types.
- ``VertexType``: Type of vertex (must inherit from ``CinderVertex``)
- ``EdgeType``: Type of edge (must inherit from ``CinderEdge``)

## Constructor
```cpp
GraphMatrix(const GraphCreationOptions &options = DEFAULT_GRAPH_OPTIONS);
```

### Parameters:
- options: (Optional) Graph configuration flags:
- ``GraphCreationOptions::Directed``
- ``GraphCreationOptions::Weighted``
- ``GraphCreationOptions::SelfLoops``
- ``GraphCreationOptions::Undirected``
- ``GraphCreationOptions::Unweighted``

# Public API Methods
### void addVertex(const VertexType &src);
Adds a vertex to the graph.

### void addEdge(const VertexType &src, const VertexType &dest);
Adds an unweighted edge between two vertices.
Throws critical error if the graph is marked as ``Weighted``.

### void addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight);
Adds a **weighted** edge between two vertices.
Throws critical error if the graph is marked as ``Unweighted``.

### EdgeType getEdge(const VertexType &src, const VertexType &dest);
Retrieves the edge (or weight) between two vertices.
Logs info and handles internal errors if retrieval fails.


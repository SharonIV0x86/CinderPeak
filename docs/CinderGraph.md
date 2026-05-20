# CinderGraph — API Reference

> **This document has been superseded by the updated documentation.**  
> Please refer to the following for complete and up-to-date information:

- **[Usage Guide](usage.md)** — Complete API reference with all method signatures, return types, and practical examples
- **[Architecture](architecture.md)** — Internal design, data flow, and component responsibilities
- **[Example Files](examples/examples.md)** — Step-by-step walkthroughs of all example programs

---

## Quick Reference

`CinderGraph<VertexType, EdgeType>` is the primary user-facing class in CinderPeak. It is a templated graph container supporting:

- Directed and undirected graphs
- Weighted edges (any `EdgeType`) and unweighted edges (`Unweighted` sentinel)
- Primitive types (`int`, `string`, `double`) and custom types (inheriting from `CinderVertex`/`CinderEdge`)

### Constructor

```cpp
CinderGraph(const GraphCreationOptions& options = GraphCreationOptions::getDefaultCreateOptions());
```

Default options: `{Directed}`

### Core Methods

| Method | Return Type | Description |
|:-------|:-----------|:------------|
| `addVertex(v)` | `pair<VertexType, bool>` | Add a vertex |
| `addEdge(src, dest)` | `pair<pair<V,V>, bool>` | Add unweighted edge (Unweighted graphs only) |
| `addEdge(src, dest, weight)` | `pair<tuple<V,V,E>, bool>` | Add weighted edge |
| `removeVertex(v)` | `bool` | Remove vertex and associated edges |
| `removeEdge(src, dest)` | `pair<optional<EdgeType>, bool>` | Remove edge |
| `updateEdge(src, dest, newW)` | `pair<EdgeType, bool>` | Update edge weight (returns newWeight) |
| `getEdge(src, dest)` | `optional<EdgeType>` | Get edge weight safely |
| `getNeighbors(v)` | `vector<pair<V, E>>` | Get all neighbors |
| `hasVertex(v)` | `bool` | Check vertex existence |
| `numVertices()` | `size_t` | Vertex count |
| `numEdges()` | `size_t` | Edge count |
| `clearVertices()` | `void` | Remove all vertices and edges |
| `clearEdges()` | `void` | Remove all edges, keep vertices |
| `toDot(filename)` | `void` | Export to Graphviz DOT |
| `bfs(src)` | `BFSResult<V>` | BFS traversal (currently stub) |
| `getGraphStatistics()` | `string` | Runtime stats summary |
| `setGraphName(name)` | `bool` | Set alphanumeric name (1-32 chars) |
| `getGraphName()` | `string` | Get graph name |
| `setConsoleLogging(bool)` | `void` | Toggle console logging |
| `setFileLogging(path)` | `void` | Enable file logging |
| `unsetFileLogging()` | `void` | Disable file logging |
| `setThrowExceptions(bool)` | `void` | Toggle exception throwing |
| `operator[](v)` | `CinderGraphRowProxy` | Matrix-style access |

For detailed usage with examples, see the [Usage Guide](usage.md).
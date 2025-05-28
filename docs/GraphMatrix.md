# CinderPeak GraphMatrix Documentation

## Overview

The `GraphMatrix` class, part of the `CinderPeak` namespace, provides a matrix-based graph data structure for managing vertices and edges. It supports both weighted and unweighted graphs, as well as directed and undirected configurations, based on options specified during graph creation. The implementation leverages a `PeakStore` backend for underlying storage and operations, ensuring efficient management of graph data.

The `GraphMatrix` class is templated to allow customization of vertex (`VertexType`) and edge (`EdgeType`) data types, making it suitable for applications like network modeling, social graphs, or weighted adjacency representations. It includes methods for adding vertices and edges, retrieving edge weights, and visualizing the graph. Additionally, the `EdgeAccessor` class provides a convenient operator-based interface for accessing and modifying edges using the `graph[src][dest]` syntax.

## Class Definitions

### `GraphMatrix`

```cpp
namespace CinderPeak {
template <typename VertexType, typename EdgeType>
class GraphMatrix {
private:
    std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>> peak_store;

public:
    GraphMatrix(const GraphCreationOptions &options = CinderPeak::GraphCreationOptions::getDefaultCreateOptions());
    void addVertex(const VertexType &src);
    void addEdge(const VertexType &src, const VertexType &dest);
    void addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight);
    EdgeType getEdge(const VertexType &src, const VertexType &dest) const;
    void visualize();
    EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src);
    const EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src) const;
    friend class EdgeAccessor<VertexType, EdgeType>;
};
}
```

#### Template Parameters
- `VertexType`: The data type for vertices (e.g., `int`, `std::string`, or a custom type like `CustomVertex`).
- `EdgeType`: The data type for edge weights (e.g., `int`, `double`, or a custom type like `CustomEdge`). For unweighted graphs, this type is required but ignored in edge operations.

### `EdgeAccessor` (Not for API use)

```cpp
namespace CinderPeak {
template <typename VertexType, typename EdgeType>
class EdgeAccessor {
private:
    GraphMatrix<VertexType, EdgeType> &graph;
    const VertexType &src;

public:
    EdgeAccessor(GraphMatrix<VertexType, EdgeType> &g, const VertexType &s);
    class EdgeReference {
    private:
        GraphMatrix<VertexType, EdgeType> &graph;
        VertexType src, dest;
    public:
        EdgeReference(GraphMatrix<VertexType, EdgeType> &g, const VertexType &s, const VertexType &d);
        EdgeReference &operator=(const EdgeType &weight);
        operator EdgeType() const;
    };
    EdgeReference operator[](const VertexType &dest);
    EdgeType operator[](const VertexType &dest) const;
};
}
```

#### Purpose
The `EdgeAccessor` class enables intuitive edge access and modification using the `graph[src][dest]` syntax. It provides:
- Read access to edge weights via `graph[src][dest]`.
- Write access to add or update edges via `graph[src][dest] = weight`.

## Constructor

### `GraphMatrix(const GraphCreationOptions &options)`
- **Description**: Initializes a new graph with the specified creation options. If no options are provided, default options are used (as defined by `GraphCreationOptions::getDefaultCreateOptions()`).
- **Parameters**:
  - `options`: A `GraphCreationOptions` object specifying graph properties (e.g., directed/undirected, weighted/unweighted, self-loops).
- **Behavior**: Creates a `PeakStore` instance with metadata indicating the graph's name ("graph_matrix") and whether the vertex and edge types are primitive.

## Methods

### `void addVertex(const VertexType &src)`
- **Description**: Adds a vertex to the graph.
- **Parameters**:
  - `src`: The vertex to add, of type `VertexType`.
- **Behavior**: Attempts to add the vertex using the `PeakStore` backend. If the operation fails, an exception is handled via `Exceptions::handle_exception_map`, and the method returns early.
- **Example**: Adding a vertex `1` to a graph of integers.

### `void addEdge(const VertexType &src, const VertexType &dest)`
- **Description**: Adds an unweighted edge between two vertices.
- **Parameters**:
  - `src`: The source vertex.
  - `dest`: The destination vertex.
- **Behavior**: Checks if the graph is configured as weighted. If so, logs a critical error and returns. Otherwise, attempts to add the edge using `PeakStore`. Handles any errors via `Exceptions::handle_exception_map`.
- **Constraints**: Only valid for unweighted graphs. Calling this on a weighted graph results in an error.

### `void addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight)`
- **Description**: Adds a weighted edge between two vertices.
- **Parameters**:
  - `src`: The source vertex.
  - `dest`: The destination vertex.
  - `weight`: The weight of the edge, of type `EdgeType`.
- **Behavior**: Checks if the graph is configured as unweighted. If so, logs a critical error and returns. Otherwise, attempts to add the edge with the specified weight using `PeakStore`. Handles errors via `Exceptions::handle_exception_map`.
- **Constraints**: Only valid for weighted graphs. Calling this on an unweighted graph results in an error.

### `EdgeType getEdge(const VertexType &src, const VertexType &dest) const`
- **Description**: Retrieves the weight of the edge between two vertices.
- **Parameters**:
  - `src`: The source vertex.
  - `dest`: The destination vertex.
- **Returns**: The edge weight of type `EdgeType` if the edge exists; otherwise, a default-constructed `EdgeType` is returned if an error occurs.
- **Behavior**: Queries the `PeakStore` for the edge weight. Handles errors via `Exceptions::handle_exception_map`.

### `void visualize()`
- **Description**: Visualizes the graph using the `PeakStore` backend.
- **Behavior**: Delegates visualization to the `PeakStore::visualize` method. Logs a message indicating the call.
- **Note**: The visualization output depends on the `PeakStore` implementation (e.g., console output, graphical rendering). Not called for non-primitive vertex or edge types in examples, as per request.

### `EdgeAccessor<VertexType, EdgeType> operator[](const VertexType &src)`
- **Description**: Provides access to edges from a given source vertex using the `graph[src][dest]` syntax.
- **Parameters**:
  - `src`: The source vertex.
- **Returns**: An `EdgeAccessor` object for the specified source vertex.
- **Behavior**: Allows chaining to access or modify edges (e.g., `graph[src][dest]` for reading, `graph[src][dest] = weight` for writing).

## EdgeAccessor Methods

### `EdgeAccessor::EdgeReference operator[](const VertexType &dest)`
- **Description**: Creates an `EdgeReference` for accessing or modifying the edge from the source to the destination vertex.
- **Parameters**:
  - `dest`: The destination vertex.
- **Returns**: An `EdgeReference` object for the edge.
- **Behavior**: Enables reading (`EdgeType`) or writing (`operator=`) for the edge.

### `EdgeAccessor::EdgeType operator[](const VertexType &dest) const`
- **Description**: Retrieves the edge weight from the source to the destination vertex.
- **Parameters**:
  - `dest`: The destination vertex.
- **Returns**: The edge weight of type `EdgeType`.
- **Behavior**: Calls `GraphMatrix::getEdge` to fetch the weight.

### `EdgeAccessor::EdgeReference::operator EdgeType() const`
- **Description**: Converts an `EdgeReference` to its corresponding edge weight.
- **Returns**: The edge weight of type `EdgeType`.
- **Behavior**: Calls `GraphMatrix::getEdge` for the stored source and destination vertices.

### `EdgeAccessor::EdgeReference &operator=(const EdgeType &weight)`
- **Description**: Assigns a weight to the edge represented by the `EdgeReference`.
- **Parameters**:
  - `weight`: The edge weight to set, of type `EdgeType`.
- **Returns**: A reference to the `EdgeReference` for chaining.
- **Behavior**: Calls `GraphMatrix::addEdge` to add or update the edge with the specified weight.

## GraphCreationOptions

The `GraphCreationOptions` class (assumed to be defined in `CinderPeak`) allows configuration of the graph's properties. Common options include:
- `Directed`: Specifies a directed graph (edges have direction).
- `Undirected`: Specifies an undirected graph (edges are bidirectional).
- `Weighted`: Specifies a weighted graph (edges have weights of type `EdgeType`).
- `Unweighted`: Specifies an unweighted graph (edges have no weights).
- `SelfLoops`: Allows edges from a vertex to itself.
- `getDefaultCreateOptions()`: Returns a default configuration (typically undirected and unweighted).

## Usage Examples

### Example 1: Custom Vertex and Edge Types
This example creates an undirected, weighted graph with custom vertex and edge types, adds vertices and edges using the operator syntax, and retrieves an edge weight. Visualization is omitted due to non-primitive types.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

class CustomVertex : public CinderVertex {
public:
    int data;
    CustomVertex() {}
};

class CustomEdge : public CinderEdge {
public:
    int dd;
    CustomEdge() {}
};

int main() {
    GraphCreationOptions options({GraphCreationOptions::Undirected,
                                  GraphCreationOptions::Weighted,
                                  GraphCreationOptions::SelfLoops});
    GraphMatrix<CustomVertex, CustomEdge> myGraph(options);

    CustomVertex v1;
    CustomVertex v2;
    CustomEdge e;
    e.dd = 1290;

    myGraph.addVertex(v1);
    myGraph.addVertex(v2);
    myGraph[v1][v2] = e;

    CustomEdge edge = myGraph.getEdge(v1, v2);
    std::cout << "Edge between v1 and v2: " << edge.dd << "\n";

    // Visualization omitted for non-primitive types
    return 0;
}
```

**Explanation**:
- Creates an undirected, weighted graph with self-loops allowed, using custom `CustomVertex` and `CustomEdge` types.
- Adds two vertices (`v1`, `v2`).
- Adds an edge from `v1` to `v2` with weight `e` (where `e.dd = 1290`) using operator syntax.
- Retrieves and prints the edge weight.
- Visualization is commented out due to non-primitive types.

### Example 2: Weighted Directed Graph with Operator Syntax
This example demonstrates a directed, weighted graph with integer vertices and edges, using the operator syntax for edge operations.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed, GraphCreationOptions::Weighted});
    GraphMatrix<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    graph[1][2] = 5;
    graph[2][3] = 10;

    int weight = graph[1][2];
    std::cout << "Weight of edge 1->2: " << weight << std::endl;

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates a directed, weighted graph with integer vertices and edges.
- Adds vertices `1`, `2`, and `3`.
- Adds weighted edges (`1 -> 2` with weight `5`, `2 -> 3` with weight `10`) using operator syntax.
- Retrieves and prints the weight of the edge `1 -> 2`.
- Visualizes the graph.

### Example 3: Unweighted Undirected Graph
This example creates an undirected, unweighted graph with string vertices and uses traditional method calls.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Undirected, GraphCreationOptions::Unweighted});
    GraphMatrix<std::string, int> graph(opts);

    graph.addVertex("A");
    graph.addVertex("B");
    graph.addVertex("C");

    graph.addEdge("A", "B");
    graph.addEdge("B", "C");

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates an undirected, unweighted graph with string vertices.
- Adds vertices `"A"`, `"B"`, and `"C"`.
- Adds unweighted edges (`A - B`, `B - C`) using method calls.
- Visualizes the graph.

### Example 4: Error Handling for Invalid Edge Addition
This example demonstrates error handling when attempting to add a weighted edge to an unweighted graph.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Undirected, GraphCreationOptions::Unweighted});
    GraphMatrix<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);

    // This will log an error due to extra weight
    graph.addEdge(1, 2, 10);

    // Correctly add an unweighted edge
    graph[1][2];

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates an undirected, unweighted graph with integer vertices.
- Adds vertices `1` and `2`.
- Attempts to add a weighted edge, which logs a critical error (as the graph is unweighted).
- Correctly adds an unweighted edge using operator syntax.
- Visualizes the graph.

## Notes
- **EdgeAccessor Usage**: The `graph[src][dest]` syntax simplifies edge access and modification, providing an intuitive matrix-like interface.
- **Error Handling**: The `GraphMatrix` class uses `Exceptions::handle_exception_map` to manage errors from `PeakStore` operations. Ensure that the `Exceptions` namespace is properly configured.
- **Type Safety**: The `VertexType` and `EdgeType` must be compatible with `PeakStore` and support default construction (for `getEdge` error cases).
- **Visualization**: The `visualize` method's output depends on the `PeakStore` implementation. It is omitted in examples with non-primitive types as requested.
- **Custom Types**: When using custom vertex or edge types (e.g., `CustomVertex`, `CustomEdge`), ensure they inherit from `CinderVertex` or `CinderEdge` and are compatible with `PeakStore`.

## Dependencies
- `CinderPeak.hpp`: Provides `GraphCreationOptions`, `PeakStore`, `CinderVertex`, `CinderEdge`, and related utilities.
- `StorageEngine/Utils.hpp`: Likely provides utility functions like `isTypePrimitive`.
- Standard C++ libraries: `iostream`, `memory` (for `std::unique_ptr`).

## Limitations
- The `EdgeType` parameter is required even for unweighted graphs, though it is unused in such cases.
- The visualization output is implementation-dependent and may require additional setup for graphical rendering.
- Error messages are logged but not propagated to the caller, relying on `Exceptions::handle_exception_map`.
- Custom vertex and edge types must be carefully designed to work with `PeakStore` operations.

This documentation provides a complete reference for using the `GraphMatrix` and `EdgeAccessor` classes, with examples covering common use cases and edge cases.
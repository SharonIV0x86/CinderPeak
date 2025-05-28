# CinderPeak GraphList Documentation

## Overview

The `GraphList` class, part of the `CinderPeak` namespace, provides a flexible and type-safe graph data structure for managing vertices and edges. It supports both weighted and unweighted graphs, as well as directed and undirected configurations, based on options specified during graph creation. The implementation leverages a `PeakStore` backend to handle the underlying storage and operations, ensuring efficient vertex and edge management.

The `GraphList` class is templated to allow customization of vertex (`VertexType`) and edge (`EdgeType`) data types, making it versatile for various applications, such as social networks, road networks, or dependency graphs. It includes methods for adding vertices and edges, retrieving edge weights, and visualizing the graph.

## Class Definition

```cpp
namespace CinderPeak {
template <typename VertexType, typename EdgeType>
class GraphList {
private:
    std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>> peak_store;

public:
    GraphList(const GraphCreationOptions &options = CinderPeak::GraphCreationOptions::getDefaultCreateOptions());
    void addVertex(const VertexType &v);
    void addEdge(const VertexType &src, const VertexType &dest);
    void addEdge(const VertexType &src, const VertexType &dest, const EdgeType &weight);
    EdgeType getEdge(const VertexType &src, const VertexType &dest);
    void visualize();
};
}
```

### Template Parameters
- `VertexType`: The data type for vertices (e.g., `int`, `std::string`, or a custom type).
- `EdgeType`: The data type for edge weights (e.g., `int`, `double`, or a custom type). For unweighted graphs, this type is still required but ignored in edge operations.

## Constructor

### `GraphList(const GraphCreationOptions &options)`
- **Description**: Initializes a new graph with the specified creation options. If no options are provided, default options are used (as defined by `GraphCreationOptions::getDefaultCreateOptions()`).
- **Parameters**:
  - `options`: A `GraphCreationOptions` object specifying graph properties (e.g., directed/undirected, weighted/unweighted).
- **Behavior**: Creates a `PeakStore` instance with metadata indicating the graph's name ("graph_list") and whether the vertex and edge types are primitive.

## Methods

### `void addVertex(const VertexType &v)`
- **Description**: Adds a vertex to the graph.
- **Parameters**:
  - `v`: The vertex to add, of type `VertexType`.
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
- **Behavior**: Checks if the graph is configured as unweighted. If so, logs a critical error and returns. Otherwise, attempts to add the edge with the specified weight using `PeakStore`. Logs a message and handles errors via `Exceptions::handle_exception_map`.
- **Constraints**: Only valid for weighted graphs. Calling this on an unweighted graph results in an error.

### `EdgeType getEdge(const VertexType &src, const VertexType &dest)`
- **Description**: Retrieves the weight of the edge between two vertices.
- **Parameters**:
  - `src`: The source vertex.
  - `dest`: The destination vertex.
- **Returns**: The edge weight of type `EdgeType` if the edge exists; otherwise, a default-constructed `EdgeType` is returned if an error occurs.
- **Behavior**: Queries the `PeakStore` for the edge weight. Logs a message and handles errors via `Exceptions::handle_exception_map`.

### `void visualize()`
- **Description**: Visualizes the graph using the `PeakStore` backend.
- **Behavior**: Delegates visualization to the `PeakStore::visualize` method. Logs a message indicating the call.
- **Note**: The exact visualization output depends on the `PeakStore` implementation (e.g., console output, graphical rendering).

## GraphCreationOptions

The `GraphCreationOptions` class (assumed to be defined in `CinderPeak`) allows configuration of the graph's properties. Common options include:
- `Directed`: Specifies a directed graph (edges have direction).
- `Undirected`: Specifies an undirected graph (edges are bidirectional).
- `Weighted`: Specifies a weighted graph (edges have weights of type `EdgeType`).
- `Unweighted`: Specifies an unweighted graph (edges have no weights).
- `getDefaultCreateOptions()`: Returns a default configuration (typically undirected and unweighted).

## Usage Examples

### Example 1: Basic Weighted Directed Graph
This example creates a directed, weighted graph with integer vertices and edges, adds vertices and edges, and visualizes the graph.

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed, GraphCreationOptions::Weighted});
    GraphList<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addVertex(4);
    graph.addVertex(5);

    graph.addEdge(1, 3, 10);
    graph.addEdge(1, 4, 9);
    graph.addEdge(4, 5, 7);
    graph.addEdge(1, 2, 6);

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates a directed, weighted graph.
- Adds vertices `1` to `5`.
- Adds weighted edges (e.g., `1 -> 3` with weight `10`).
- Visualizes the graph structure.

### Example 2: Unweighted Undirected Graph
This example demonstrates an undirected, unweighted graph with string vertices.

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Undirected, GraphCreationOptions::Unweighted});
    GraphList<std::string, int> graph(opts);

    graph.addVertex("A");
    graph.addVertex("B");
    graph.addVertex("C");
    graph.addVertex("D");

    graph.addEdge("A", "B");
    graph.addEdge("B", "C");
    graph.addEdge("C", "D");
    graph.addEdge("D", "A");

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates an undirected, unweighted graph with string vertices.
- Adds vertices labeled `"A"`, `"B"`, `"C"`, and `"D"`.
- Adds unweighted edges to form a cycle (`A-B-C-D-A`).
- Visualizes the graph.

### Example 3: Weighted Graph with Edge Retrieval
This example creates a weighted graph, adds edges, and retrieves an edge weight.

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed, GraphCreationOptions::Weighted});
    GraphList<int, double> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    graph.addEdge(1, 2, 5.5);
    graph.addEdge(2, 3, 3.2);

    double weight = graph.getEdge(1, 2);
    std::cout << "Weight of edge 1->2: " << weight << std::endl;

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates a directed, weighted graph with integer vertices and double-precision edge weights.
- Adds vertices `1`, `2`, and `3`.
- Adds weighted edges (`1 -> 2` with weight `5.5`, `2 -> 3` with weight `3.2`).
- Retrieves and prints the weight of the edge `1 -> 2`.
- Visualizes the graph.

### Example 4: Error Handling for Invalid Edge Addition
This example demonstrates error handling when attempting to add an unweighted edge to a weighted graph.

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed, GraphCreationOptions::Weighted});
    GraphList<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);

    // This will log an error due to missing weight
    graph.addEdge(1, 2);

    // Correctly add a weighted edge
    graph.addEdge(1, 2, 10);

    graph.visualize();
    return 0;
}
```

**Explanation**:
- Creates a directed, weighted graph.
- Adds vertices `1` and `2`.
- Attempts to add an unweighted edge, which logs a critical error (as the graph is weighted).
- Correctly adds a weighted edge (`1 -> 2` with weight `10`).
- Visualizes the graph.

## Notes
- **Error Handling**: The `GraphList` class uses `Exceptions::handle_exception_map` to manage errors from `PeakStore` operations. Ensure that the `Exceptions` namespace is properly configured to handle errors gracefully.
- **Type Safety**: The `VertexType` and `EdgeType` must be compatible with the `PeakStore` backend and support operations like default construction (for `getEdge` error cases).
- **Visualization**: The `visualize` method's output depends on the `PeakStore` implementation, which may vary (e.g., text-based, graphical).
- **Constraints**: Ensure that edge operations match the graph's weighted/unweighted configuration to avoid runtime errors.

## Dependencies
- `CinderPeak.hpp`: Provides `GraphCreationOptions`, `PeakStore`, and related utilities.
- `StorageEngine/Utils.hpp`: Likely provides utility functions like `isTypePrimitive`.
- Standard C++ libraries: `iostream`, `memory` (for `std::unique_ptr`).

## Limitations
- The `EdgeType` parameter is required even for unweighted graphs, though it is unused in such cases.
- The visualization output is implementation-dependent and may require additional setup for graphical rendering.
- Error messages are logged but not propagated to the caller, relying on `Exceptions::handle_exception_map`.

This documentation provides a complete reference for using the `GraphList` class, with examples covering common use cases and edge cases.
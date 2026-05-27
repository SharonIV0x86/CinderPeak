# Beginner Guide to CinderPeak

## What is a Graph?

A graph is a data structure made of:
- Vertices (nodes)
- Edges (connections between nodes)

Graphs are commonly used in:
- Social networks
- Maps and navigation
- Recommendation systems
- Network analysis

---

## Basic Example

```cpp
#include <iostream>
#include "CinderPeak.hpp"

using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed});

    CinderGraph<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);

    graph.addEdge(1, 2, 10);

    std::cout << "Vertices: " << graph.numVertices() << std::endl;
    std::cout << "Edges: " << graph.numEdges() << std::endl;

    return 0;
}
```

---

## Explanation

- `addVertex()` adds a node to the graph.
- `addEdge()` creates a connection between two nodes.
- `numVertices()` returns the total number of vertices.
- `numEdges()` returns the total number of edges.

---

## Weighted vs Unweighted Graphs

- Weighted graphs store values on edges (distance, cost, etc.)
- Unweighted graphs only store connections.

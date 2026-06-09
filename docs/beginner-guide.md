# Beginner Guide to CinderPeak

## What is CinderPeak?

CinderPeak is a modern, open-source C++ graph library. It uses a fully templated design, meaning you can define your own vertex and edge types — or just use simple types like `int`. It supports directed and undirected graphs, weighted and unweighted edges, and is built for performance using C++17/20 features.

---

## Installation

CinderPeak uses CMake. Follow the [installation guide](../docs/installation.md) to set it up, then include the header:

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;
```

---

## Your First Graph (Simple)

The simplest way to get started is with `int` vertices and edges:

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Directed});
    CinderGraph<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 10);       // edge from 1 to 2, weight 10
    graph.updateEdge(1, 2, 20);    // update weight to 20

    std::cout << "Vertices: " << graph.numVertices() << "\n";
    std::cout << "Edges: " << graph.numEdges() << "\n";
}
```

---

## Unweighted Graphs

If your graph doesn't need edge weights, use the `Unweighted` type:

```cpp
GraphCreationOptions opts({GraphCreationOptions::Directed});
CinderGraph<int, Unweighted> graph(opts);

graph.addVertex(1);
graph.addVertex(2);
graph.addEdge(1, 2);   // no weight needed
```

---

## Custom Vertex and Edge Types

CinderPeak shines when you define your own types. Your structs must extend `CinderVertex` and `CinderEdge`:

```cpp
struct MyVertex : CinderPeak::CinderVertex {
    int id;
    MyVertex(int id) : id(id) {}
};

struct MyEdge : CinderPeak::CinderEdge {
    float weight;
    MyEdge(float w) : weight(w) {}
};

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Undirected});
    CinderGraph<MyVertex, MyEdge> graph(opts);

    MyVertex v1(1), v2(2);
    MyEdge e1(0.5f), e2(0.8f);

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addEdge(v1, v2, e1);
    graph.updateEdge(v1, v2, e2);  // update edge to new weight
}
```

---

## Key API Reference

| Method | Description |
|--------|-------------|
| `addVertex(v)` | Adds a vertex to the graph |
| `addEdge(u, v, e)` | Adds an edge between two vertices |
| `updateEdge(u, v, e)` | Updates an existing edge |
| `numVertices()` | Returns total vertex count |
| `numEdges()` | Returns total edge count |

---

## Next Steps

- Read the [Usage Guide](../docs/usage.md) for full API details
- Explore the `examples/` directory in the repo
- Check out the full docs hosted with Docusaurus
Closes #[299]

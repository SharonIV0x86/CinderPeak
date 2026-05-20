# 🌋 CinderPeak Documentation

Welcome to the official documentation for **CinderPeak** — a high-performance, modular C++ graph library designed for modern systems programming and real-world graph applications.

CinderPeak is built for **speed**, **type-safety**, and **ease of use**. It supports multiple graph storage backends (Adjacency List, Hybrid CSR/COO), flexible vertex and edge types, thread-safe operations, and rich runtime configuration.

---

## 📖 Table of Contents

| Document | Description |
|:---------|:------------|
| 📦 [Installation Guide](installation.md) | Step-by-step build instructions for Linux, macOS, and Windows |
| 🏗️ [Architecture](architecture.md) | Internal library design, layers, data flow, and design patterns |
| ⚙️ [Usage Guide](usage.md) | Complete API reference with practical, beginner-friendly examples |
| 🔧 [Development Guide](development.md) | Contributing, testing, and extending the library |
| 📁 [Example Files](examples/) | Documented walkthroughs of all example programs |

---

## ✨ Features

- 🚀 **High-performance** in-memory graph processing
- 🧩 **Modular storage engine** — Adjacency List and Hybrid CSR/COO backends
- 🔀 **Directed & Undirected** graph support
- ⚖️ **Weighted & Unweighted** edge configurations
- 🧵 **Thread-safe** — `std::shared_mutex` and `std::atomic` throughout
- 🔡 **Flexible types** — Primitive types (`int`, `string`) and custom user-defined types
- 📊 **Graph statistics** — built-in density, vertex/edge counts, self-loop counts
- 🖨️ **DOT export** — visualize graphs with Graphviz-compatible output
- 📋 **Verbose logging** — configurable console and file-based logging
- 🛡️ **Exception handling** — configurable throw/no-throw error policy

---

## 🚀 Quick Start

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    // Create a directed weighted graph with integer vertices and edges
    GraphCreationOptions opts({GraphCreationOptions::Directed});
    CinderGraph<int, int> graph(opts);

    // Add vertices
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    // Add weighted edges
    graph.addEdge(1, 2, 10);
    graph.addEdge(2, 3, 20);

    // Query neighbors
    auto neighbors = graph.getNeighbors(1);
    for (auto& [vertex, weight] : neighbors) {
        std::cout << "1 -> " << vertex << " (weight: " << weight << ")\n";
    }

    // Export to Graphviz DOT format
    graph.toDot("my_graph.dot");

    return 0;
}
```

Expected output:
```
1 -> 2 (weight: 10)
```

---

## 🗂️ Project Structure

```
CinderPeak/
├── src/                          # Library source headers
│   ├── CinderPeak.hpp            # Main public include (use this)
│   ├── CinderGraph.hpp           # User-facing graph class
│   ├── PeakStore.hpp             # Internal storage orchestrator
│   ├── Concepts.hpp              # Compile-time type traits
│   ├── GraphConstraints.hpp      # Edge/vertex constraint enforcement
│   ├── GraphEvents.hpp           # Event hooks (edge/vertex add/remove)
│   ├── GraphRuntime.hpp          # Runtime config (logging, exceptions)
│   ├── PeakLogger.hpp            # Logging subsystem
│   ├── StorageEngine/
│   │   ├── AdjacencyList.hpp     # Adjacency list storage backend
│   │   ├── HybridCSR_COO.hpp     # Hybrid CSR+COO storage backend
│   │   ├── GraphContext.hpp      # Shared context object
│   │   ├── GraphStatistics.hpp   # Metadata and statistics tracking
│   │   ├── Utils.hpp             # Core types and utilities
│   │   ├── ErrorCodes.hpp        # Status codes and PeakStatus
│   │   └── DebugUtils.hpp        # Debug string helpers
│   └── Algorithms/
│       └── CinderPeakAlgorithms.hpp  # Algorithm stubs (BFS, future work)
├── examples/
│   ├── CinderGraph/              # Per-API example programs
│   └── Algorithms/               # Algorithm usage examples
├── docs/                         # This documentation
├── tests/                        # Google Test unit tests
└── CMakeLists.txt                # Build system configuration
```

---

## 🙌 Contributing

Contributions are always welcome! Please read [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines on:
- Reporting bugs
- Suggesting features
- Submitting pull requests
- Running tests

---

## 📬 Contact

Maintained by the CinderPeak team.  
For questions or issues, open an issue on [GitHub](https://github.com/SharonIV0x86/CinderPeak).

---

Happy graphing! 🧠📊

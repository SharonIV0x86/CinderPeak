# CinderPeak: A Modern C++ Graph Library

A fast and efficient, open-source C++ graph library built to handle a wide range of graph types. It provides a flexible, templated API for graph manipulation, analysis.

A fast and efficient, open-source C++ graph library built to handle a wide range of graph types. 
It provides a flexible, templated API for graph manipulation and analysis.

![License](https://img.shields.io/badge/license-MIT-green)
![C++](https://img.shields.io/badge/Built%20with-C%2B%2B17%2FC%2B%2B20-blue)
![Dependencies](https://img.shields.io/badge/Dependencies-GTest-red)

---

## Table of Contents
- 🚀 Key Features
- 🗓️ Development Notice
- 📂 Project Structure
- ⚙️ Getting Started
- 🛠️ Technology Stack
- ❓ Why CinderPeak?
- 🧑‍💻 Community & Contributions
- 📄 License

---

## Key Features
- **Flexible Graph Representations** - Supports adjacency lists, adjacency matrices, and hybrid CSR/COO formats for efficient storage.
- **Customizable & Templated** - Fully templated design allows you to define custom vertex and edge types.
- **Thread Safety** - Designed to work seamlessly in multi-threaded applications.
- **High Performance** - Leverages modern C++ features like smart pointers and STL containers for optimized execution.
- **Comprehensive Testing** - Built with Google Test (GTest) to ensure reliability and robustness.
- **Extensive Documentation** - Detailed usage guides, examples, and API references are hosted with Docusaurus.

---
# Example
```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak;

// Custom vertex/edge types
struct MyVertex : CinderPeak::CinderVertex {
  int id;
  MyVertex(int id) : id(id) {}
};
struct MyEdge : CinderPeak::CinderEdge {
  float w;
  MyEdge(float w) : w(w) {}
};

int main() {
  // --- Complex Graph (custom vertex + weighted edge) ---
  GraphCreationOptions optsU({GraphCreationOptions::Undirected});
  PolicyConfiguration cfg(PolicyConfiguration::Throw,
                          PolicyConfiguration::Silent);

  GraphList<MyVertex, MyEdge> customGraph(optsU, cfg);

  MyVertex v1(1);
  MyVertex v2(2);
  
  MyEdge e1(0.5f);
  MyEdge e2(0.8f);

  customGraph.addVertex(v1);
  customGraph.addVertex(v2);
  customGraph.addEdge(v1, v2, e1);
  customGraph.updateEdge(v1, v2, e2);

  // --- Simple Weighted Graph (int vertices/edges) ---
  GraphCreationOptions optsD({GraphCreationOptions::Directed});
  PolicyConfiguration cfg(PolicyConfiguration::Throw,
                          PolicyConfiguration::LogConsole);

  GraphList<int, int> intGraph(optsD);

  intGraph.addVertex(1);
  intGraph.addVertex(2);
  intGraph.addEdge(1, 2, 10);
  intGraph.updateEdge(1, 2, 20);

  std::cout << "Vertices: " << intGraph.numVertices()
            << ", Edges: " << intGraph.numEdges() << "\n";

  // --- Unweighted Graph ---
  GraphList<int, Unweighted> unweighted(optsD);
  unweighted.addVertex(1);
  unweighted.addVertex(2);
  unweighted.addEdge(1, 2);
}
```


---
## Development Notice

CinderPeak is currently under active development. We are committed to delivering a polished and comprehensive release. The stable version, with refined functionalities and complete documentation, is scheduled to be available soon.

---

## Project Structure
```
/CinderPeak
├── CMakeLists.txt              # Build system configuration
├── docs                        # Docusaurus documentation
│   ├── examples
│   │   └── GraphMatrixExample.md  # Example usage for GraphMatrix
│   ├── GraphList.md            # Adjacency List documentation
│   ├── GraphMatrix.md          # Adjacency Matrix documentation
│   ├── index.md                # Main documentation page
│   ├── installation.md         # Installation guide
│   └── usage.md                # Usage guide
├── examples                    # Sample code demonstrating usage
│   ├── CMakeLists.txt          # Build config for examples
│   ├── extras
│   │   ├── COOExample.cpp      # Coordinate List example
│   │   ├── CSRExample.cpp      # Compressed Sparse Row example
│   │   ├── LogExample.cpp      # Logging utility example
│   │   └── PeakExample.cpp     # General CinderPeak usage example
│   ├── ListExample1.cpp        # Adjacency List example
│   ├── MatrixExample.cpp       # Adjacency Matrix example
│   └── PrimitiveGraph.cpp      # Basic graph example
├── src                         # Source files
│   ├── CinderExceptions.hpp    # Custom exception handling
│   ├── CinderPeak.hpp          # Main API entry point
│   ├── GraphList.hpp           # Adjacency List implementation
│   ├── GraphMatrix.hpp         # Adjacency Matrix implementation
│   ├── PeakLogger.hpp          # Logging utility
│   ├── PeakStore.hpp           # Core storage engine
│   ├── StorageEngine
│   │   ├── AdjacencyList.hpp   # Adjacency List storage
│   │   ├── CoordinateList.hpp  # Coordinate List storage
│   │   ├── ErrorCodes.hpp      # Error handling codes
│   │   ├── GraphContext.hpp    # Graph context management
│   │   ├── HybridCSR_COO.hpp   # Hybrid CSR/COO storage
│   │   └── Utils.hpp           # Utility functions
│   ├── StorageInterface.hpp    # Storage interface definition
├── tests                       # Unit tests
│   ├── AdjacencyShard.cpp      # Tests for adjacency list
│   ├── CoordinateShard.cpp     # Tests for coordinate list
│   ├── HybridShard.cpp         # Tests for hybrid CSR/COO
│   └── tests.cpp               # Main test suite
├── README.md                   # Project overview and setup
└── LICENSE                     # License file
```
---

## Getting Started
1. **Installation**: Follow the [installation guide](docs/installation.md) to set up CinderPeak with CMake.
2. **Usage**: Check the [usage guide](docs/usage.md) for API details and the [examples](examples/) directory for sample code.
3. **Documentation**: Explore the full documentation hosted with Docusaurus in the [docs](docs/) directory.

---

## Technology Stack
- **C++17/C++20**: Leverages modern C++ features for performance and flexibility.
- **Google Test**: Provides the framework for robust unit testing.
- **Docusaurus**: Hosts comprehensive documentation with examples and API references.
- **CMake**: Used for the cross-platform build system.

---

## Why CinderPeak?
CinderPeak strikes a balance between **performance, flexibility, and ease of use**. Whether you're building complex network models, analyzing graph-based data, or managing relationships, CinderPeak provides a robust and intuitive solution. Its open-source nature encourages community contributions, and its modular design makes it easy to extend for specialized use cases.

---

## Community & Contributions
We welcome contributions! See the [CONTRIBUTING.md](CONTRIBUTING.md) file for guidelines on how to get involved. Join the CinderPeak community on [GitHub](https://github.com/SharonIV0X86/CinderPeak) to report issues, suggest features, or contribute code.

---

## 🌟 Contributors
<a href="https://github.com/SharonIV0X86/CinderPeak/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=SharonIV0X86/CinderPeak" />
</a>

---

## License
This project is licensed under the [MIT License](./License).

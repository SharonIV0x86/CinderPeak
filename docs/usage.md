# **CinderPeak: A Practical Usage Guide**

Welcome to the CinderPeak community! This guide is designed to be a practical, beginner-friendly resource to help you familiarize yourself with the basic project functionality. This guide provides the usage instructions of how to include the header files, and which classes and structures are available for users to use and how can you use them to get started with the project with some basic usage examples.

### **Quick Links**

* [**Project Index**](https://github.com/SharonIV0x86/CinderPeak/blob/main/docs/index.md) 
* [**Installation Guide**](https://github.com/SharonIV0x86/CinderPeak/blob/main/docs/installation.md)

---

## **The CinderPeak API**

### **Library Headers**

CinderPeak's public API is modular, with components split into separate headers. The table below highlights the key ones.

| Header File | Key Classes/Structures | Purpose |
| :---- | :---- | :---- |
| CinderPeak.hpp | (Convenience Header) | Includes all necessary public headers for basic use. This should be your default choice. |
| GraphMatrix.hpp | GraphMatrix | The concrete implementation of a graph using an adjacency matrix data structure. |Offering matrix like edge and vertex access.
| GraphList.hpp | GraphList | The concrete implementation of a graph using an adjacency list data structure. |
| PeakStore.hpp | PeakStore | Core storage engine that manages graph data internally. |
| StorageEngine/Utils.hpp | GraphCreationOptions, CinderVertex, CinderEdge | Contains utility functions, graph creation options, and base classes for custom types. |

## **How to Include Header Files**

### CinderPeak.hpp
To use CinderPeak, you need only to include this header file along with the required namespace declarations.

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;
```

**Purpose and Importance:**  
This main public API header serves as a single entry point, bundling all library components. It simplifies setup by removing the need for multiple includes. While the core logic is in /src, you'll mostly interact with this header.

### Individual Headers (Advanced Usage)

If you want to include specific components only:

```cpp
#include "GraphMatrix.hpp"  // For adjacency matrix graphs
#include "GraphList.hpp"    // For adjacency list graphs
#include "StorageEngine/Utils.hpp"  // For utility functions and options
```

**Purpose and Importance:**  
- `GraphMatrix.hpp`: Best for dense graphs, where checking edge existence is frequent
- `GraphList.hpp`: Adjacency list implementation. Efficient for sparse graphs, supports directed/undirected edges, allows dynamic updates, and suits DFS/BFS/shortest paths
- `StorageEngine/Utils.hpp`: Provides common utilities for the library, including type checks, graph creation options, metadata, and constants

---

## **Core Classes and Structures**

CinderPeak's architecture is built on a few key components that you'll interact with regularly. It is designed around a small set of key classes that define how graphs are represented, created, and manipulated.

| Class / Structure | Header Location | Purpose |
| :---- | :---- | :---- |
| GraphMatrix | src/GraphMatrix.hpp | An adjacency matrix implementation of a graph. Fast for checking edge existence, ideal for dense graphs (many edges). |
| GraphList | src/GraphList.hpp | An adjacency list implementation of a graph. Efficient for sparse graphs. |
| PeakStore | src/PeakStore.hpp | Core storage engine that handles the internal graph data management. |
| GraphCreationOptions | src/StorageEngine/Utils.hpp | Configuration class that defines how a graph should behave (Directed, Weighted, etc.). |
| CinderVertex | src/StorageEngine/Utils.hpp | Base class for custom vertex types. |
| CinderEdge | src/StorageEngine/Utils.hpp | Base class for custom edge types. |

### **GraphMatrix**
- Implements adjacency matrix representation
- Best for dense graphs or when edge lookups must be very fast
- Higher memory cost but simpler constant-time edge queries
- Direct instantiation: `GraphMatrix<VertexType, EdgeType> graph(options)`

### **GraphList**
- Implements adjacency list representation
- Best for sparse graphs or when graphs are frequently updated
- Efficient for traversals like BFS/DFS
- Direct instantiation: `GraphList<VertexType, EdgeType> graph(options)`

### **PeakStore**
- Manages internal storage of graph data, ensuring efficient data handling across graph types.
- Typically used internally but essential for understanding the library’s architecture.

### **GraphCreationOptions**
Defines how a graph should behave (Directed, Undirected, Weighted, Unweighted). Available options include:
- `GraphCreationOptions::Directed`
- `GraphCreationOptions::Undirected`
- `GraphCreationOptions::Weighted`
- `GraphCreationOptions::Unweighted`
- `GraphCreationOptions::SelfLoops`
- `GraphCreationOptions::ParallelEdges`

### **CinderVertex and CinderEdge**
Base classes that custom vertex and edge types must inherit from. They provide:
- Unique ID generation
- Name generation
- Comparison operators
- Hash support for use in containers

---

## **Using Custom Data Types**

CinderPeak is a fully templated library, which means you can define your own struct or class for vertex and edge data. However, custom types must inherit from the provided base classes.

**Example: Defining Custom Classes for a Social Network**

```cpp
#include <string>
#include "CinderPeak.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

// A custom class to hold data for each person (vertex)
class UserProfile : public CinderVertex {
public:
    std::string username;
    int age;
    
    UserProfile() {}
    UserProfile(const std::string& name, int userAge) : username(name), age(userAge) {}
};

// A custom class to hold data for each friendship (edge)
class Friendship : public CinderEdge {
public:
    int year_met;
    std::string relationship_type; // e.g., "Family", "Work"
    
    Friendship() {}
    Friendship(int year, const std::string& type) : year_met(year), relationship_type(type) {}
};
```

---

## **Basic Usage: Code Examples**

Here are practical, step-by-step examples showing how to use the library's core functionality.

### **Example 1: Creating a Simple Integer Graph**

This example shows how to create a basic directed, weighted graph storing integers.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

int main() {
    // Create graph creation options
    GraphCreationOptions opts({
        GraphCreationOptions::Directed,
        GraphCreationOptions::Weighted
    });

    // Create a graph that stores integers for both vertices and edges
    GraphMatrix<int, int> graph(opts);

    // Add vertices to the graph
    graph.addVertex(10);
    graph.addVertex(20);
    graph.addVertex(30);

    // Add weighted edges to connect the vertices
    graph.addEdge(10, 20, 5);   // Edge from 10 to 20 with weight 5
    graph.addEdge(20, 30, 10);  // Edge from 20 to 30 with weight 10

    // Access edge weights using operator[]
    std::cout << "Edge weight from 10 to 20: " << graph[10][20] << std::endl;
    
    // Or using getEdge method
    int edgeWeight = graph.getEdge(20, 30);
    std::cout << "Edge weight from 20 to 30: " << edgeWeight << std::endl;

    return 0;
}
```

### **Example 2: Using Custom Data Types**

This example uses the UserProfile and Friendship classes defined earlier to create a directed social network graph.

```cpp
#include <iostream>
#include <string>
#include "CinderPeak.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

// Custom classes (defined as shown in previous section)
class UserProfile : public CinderVertex {
public:
    std::string username;
    int age;
    UserProfile() {}
    UserProfile(const std::string& name, int userAge) : username(name), age(userAge) {}
};

class Friendship : public CinderEdge {
public:
    int year_met;
    std::string relationship_type;
    Friendship() {}
    Friendship(int year, const std::string& type) : year_met(year), relationship_type(type) {}
};

int main() {
    // Create a directed, weighted graph with custom types
    GraphCreationOptions opts({
        GraphCreationOptions::Directed,
        GraphCreationOptions::Weighted
    });
    
    GraphMatrix<UserProfile, Friendship> socialGraph(opts);

    // Create user profiles
    UserProfile alice("Alice", 30);
    UserProfile bob("Bob", 25);
    UserProfile charlie("Charlie", 35);

    // Add vertices to the graph
    socialGraph.addVertex(alice);
    socialGraph.addVertex(bob);
    socialGraph.addVertex(charlie);

    // Create friendship connections
    Friendship workFriend(2022, "Work");
    Friendship familyMember(2020, "Family");

    // Add edges with friendship data
    socialGraph.addEdge(alice, bob, workFriend);
    socialGraph.addEdge(alice, charlie, familyMember);

    // Access edge data
    Friendship connection = socialGraph.getEdge(alice, bob);
    std::cout << "Alice and Bob met in: " << connection.year_met << std::endl;
    std::cout << "Relationship type: " << connection.relationship_type << std::endl;

    return 0;
}
```

### **Example 3: Using GraphList for Sparse Graphs**

This example demonstrates using the adjacency list representation for a sparse graph.

```cpp
#include <iostream>
#include "CinderPeak.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

int main() {
    // Create options for an undirected, weighted graph
    GraphCreationOptions opts({
        GraphCreationOptions::Undirected,
        GraphCreationOptions::Weighted
    });

    // Use GraphList for sparse graphs
    GraphList<int, double> network(opts);

    // Add vertices
    network.addVertex(1);
    network.addVertex(2);
    network.addVertex(3);
    network.addVertex(4);
    network.addVertex(5);

    // Add weighted edges (distances)
    network.addEdge(1, 3, 10.5);
    network.addEdge(1, 4, 9.2);
    network.addEdge(4, 5, 7.8);
    network.addEdge(1, 2, 6.1);

    std::cout << "Successfully created a sparse network graph!" << std::endl;

    return 0;
}
```

---

## **Graph Creation Options**

When creating graphs, you can specify various options that control the graph's behavior:

```cpp
// Directed, weighted graph
GraphCreationOptions opts1({
    GraphCreationOptions::Directed,
    GraphCreationOptions::Weighted
});

// Undirected, unweighted graph with self-loops allowed
GraphCreationOptions opts2({
    GraphCreationOptions::Undirected,
    GraphCreationOptions::Unweighted,
    GraphCreationOptions::SelfLoops
});

// Directed graph with parallel edges allowed
GraphCreationOptions opts3({
    GraphCreationOptions::Directed,
    GraphCreationOptions::ParallelEdges
});
```

### **Available Options:**

| Option | Description |
| :---- | :---- |
| `Directed` | Creates a directed graph where edges have direction |
| `Undirected` | Creates an undirected graph where edges are bidirectional |
| `Weighted` | Graph supports edge weights |
| `Unweighted` | Graph doesn't use edge weights |
| `SelfLoops` | Vertices can have edges to themselves |
| `ParallelEdges` | Multiple edges between the same pair of vertices are allowed |

---

## **Core Operations**

### **Adding Elements**

```cpp
// Add vertices
graph.addVertex(data);

// Add unweighted edges (for unweighted graphs)
graph.addEdge(source, destination);

// Add weighted edges (for weighted graphs)
graph.addEdge(source, destination, weight);
```

### **Accessing Edge Data**

```cpp
// Using operator[] (GraphMatrix only)
EdgeType weight = graph[source][destination];

// Using getEdge method (both GraphMatrix and GraphList)
EdgeType weight = graph.getEdge(source, destination);
```

### **Setting Edge Data**

```cpp
// Using operator[] assignment (GraphMatrix only)
graph[source][destination] = newWeight;
```

---

## **Choosing Between GraphMatrix and GraphList**

The choice between GraphMatrix and GraphList depends on your specific use case:

| Representation | Space Complexity | Add Edge | Check Adjacency | Best For |
| :---- | :---- | :---- | :---- | :---- |
| **GraphList** | O(V+E) | O(1) amortized | O(degree(V)) | **Sparse graphs** (few edges compared to vertices), like road networks or social networks. |
| **GraphMatrix** | O(V²) | O(1) | O(1) | **Dense graphs** (many edges, approaching the maximum possible), like complete graphs or dense networks. |

**Guidelines:**
- Use `GraphList` for most common applications, especially when you have many vertices but relatively few edges
- Use `GraphMatrix` when you need very fast edge existence checks or when working with dense graphs
- For beginners, `GraphList` is usually the safer choice

---

## **Error Handling**

CinderPeak uses an internal error handling system. When operations fail, they are logged internally:

- Adding duplicate vertices (when parallel edges aren't allowed) will log a warning but won't crash
- Accessing non-existent edges returns a default-constructed EdgeType
- Most errors are handled gracefully with appropriate logging

---

## **Best Practices**

1. **Always include required namespaces:**
   ```cpp
   using namespace CinderPeak::PeakStore;
   using namespace CinderPeak;
   ```

2. **Choose the right graph representation:**
   - Use `GraphList` for sparse graphs
   - Use `GraphMatrix` for dense graphs or when you need O(1) edge lookups

3. **Custom types must inherit from base classes:**
   ```cpp
   class MyVertex : public CinderVertex { /* ... */ };
   class MyEdge : public CinderEdge { /* ... */ };
   ```

4. **Configure graph options appropriately:**
   ```cpp
   GraphCreationOptions opts({
       GraphCreationOptions::Directed,    // or Undirected
       GraphCreationOptions::Weighted     // or Unweighted
   });
   ```

5. **Use consistent data types:**
   - Make sure your vertex and edge types are consistent throughout your application
   - Consider using primitive types (int, double, string) for simple cases
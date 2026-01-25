# Graph Serialization and Deserialization

CinderPeak provides built-in support for graph serialization and deserialization, allowing you to save graph structures to files and load them back later. This feature is useful for:

- **Persistent Storage**: Save graphs across sessions
- **Data Sharing**: Share graph data with collaborators or across different applications
- **Testing**: Create and reuse test graph datasets
- **Checkpointing**: Save intermediate states during long computations

## Features

- **JSON Format**: Human-readable format for easy debugging and manual editing
- **Complete Graph State**: Saves vertices, edges, weights, and graph properties
- **Type Support**: Works with primitive types (int, float, double), strings, and custom types (CinderVertex, CinderEdge)
- **Graph Properties**: Preserves directed/undirected, weighted/unweighted, self-loops, and parallel edges settings
- **Error Handling**: Comprehensive error checking with detailed error messages

## Basic Usage

### Saving a Graph

```cpp
#include "CinderGraph.hpp"
using namespace CinderPeak;

// Create and populate a graph
CinderGraph<int, int> graph;
graph.addVertex(1);
graph.addVertex(2);
graph.addVertex(3);
graph.addEdge(1, 2, 10);
graph.addEdge(2, 3, 20);

// Save to file
auto result = graph.save("my_graph.json");
if (result.success) {
    std::cout << "Success: " << result.message << std::endl;
} else {
    std::cerr << "Error: " << result.message << std::endl;
}
```

### Loading a Graph

```cpp
// Create a new graph and load from file
CinderGraph<int, int> loadedGraph;
auto result = loadedGraph.load("my_graph.json");

if (result.success) {
    std::cout << "Graph loaded successfully!" << std::endl;
    std::cout << "Vertices: " << loadedGraph.numVertices() << std::endl;
    std::cout << "Edges: " << loadedGraph.numEdges() << std::endl;
} else {
    std::cerr << "Failed to load: " << result.message << std::endl;
}
```

## JSON File Format

The serialization format uses a structured JSON schema:

```json
{
  "graph_metadata": {
    "version": "1.0",
    "directed": true,
    "undirected": false,
    "self_loops": true,
    "parallel_edges": false,
    "weighted": true,
    "vertex_count": 3,
    "edge_count": 2
  },
  "vertices": [1, 2, 3],
  "edges": [
    {
      "source": 1,
      "target": 2,
      "weight": 10
    },
    {
      "source": 2,
      "target": 3,
      "weight": 20
    }
  ]
}
```

## Examples

### Example 1: String Graph with Weights

```cpp
GraphCreationOptions options({GraphCreationOptions::Directed});
CinderGraph<std::string, double> cityGraph(options);

// Build a city transportation network
cityGraph.addVertex("New York");
cityGraph.addVertex("Boston");
cityGraph.addVertex("Philadelphia");

cityGraph.addEdge("New York", "Boston", 215.5);
cityGraph.addEdge("New York", "Philadelphia", 95.0);
cityGraph.addEdge("Philadelphia", "Boston", 310.2);

// Save the graph
auto saveResult = cityGraph.save("cities.json");
if (saveResult.success) {
    std::cout << "City graph saved!" << std::endl;
}

// Later, load it back
CinderGraph<std::string, double> loadedCities;
auto loadResult = loadedCities.load("cities.json");

// Verify the data
auto [distance, found] = loadedCities.getEdge("New York", "Boston");
if (found && distance) {
    std::cout << "Distance NY->Boston: " << *distance << " miles" << std::endl;
}
```

### Example 2: Custom Vertex Types

```cpp
// Using CinderVertex for custom vertex types
CinderGraph<CinderVertex, int> customGraph;

CinderVertex v1("NodeA");
CinderVertex v2("NodeB");
CinderVertex v3("NodeC");

customGraph.addVertex(v1);
customGraph.addVertex(v2);
customGraph.addVertex(v3);

customGraph.addEdge(v1, v2, 100);
customGraph.addEdge(v2, v3, 200);

// Save and load
customGraph.save("custom_graph.json");

CinderGraph<CinderVertex, int> loadedCustom;
loadedCustom.load("custom_graph.json");
```

### Example 3: Graph Configuration Preservation

```cpp
// Create a graph with specific properties
GraphCreationOptions options({
    GraphCreationOptions::Undirected,
    GraphCreationOptions::SelfLoops,
    GraphCreationOptions::ParallelEdges
});

CinderGraph<int, float> specialGraph(options);

specialGraph.addVertex(1);
specialGraph.addVertex(2);
specialGraph.addEdge(1, 2, 1.5f);
specialGraph.addEdge(1, 1, 2.0f);  // Self-loop
specialGraph.addEdge(1, 2, 3.5f);  // Parallel edge

// Save preserves all graph properties
specialGraph.save("special_graph.json");

// When loaded, the graph retains all properties
CinderGraph<int, float> loadedSpecial;
loadedSpecial.load("special_graph.json");
// The loaded graph will have the same configuration
```

### Example 4: Large Graph Serialization

```cpp
CinderGraph<int, int> largeGraph;

// Create a large graph
for (int i = 0; i < 1000; ++i) {
    largeGraph.addVertex(i);
}

for (int i = 0; i < 999; ++i) {
    largeGraph.addEdge(i, i + 1, i * 10);
}

// Save large graph
auto result = largeGraph.save("large_graph.json");
if (result.success) {
    std::cout << "Saved " << largeGraph.numVertices()
              << " vertices and " << largeGraph.numEdges()
              << " edges" << std::endl;
}
```

## Error Handling

The serialization API returns a `SerializationResult` structure with detailed information:

```cpp
auto result = graph.save("output.json");

if (!result.success) {
    std::cerr << "Error occurred: " << result.message << std::endl;

    // Check the status code
    if (result.status.code() == StatusCode::FILE_IO_ERROR) {
        std::cerr << "Unable to write to file" << std::endl;
    } else if (result.status.code() == StatusCode::SERIALIZATION_ERROR) {
        std::cerr << "Failed to serialize graph data" << std::endl;
    }
}
```

### Common Error Scenarios

1. **File I/O Errors**: Invalid file path or insufficient permissions

   ```cpp
   auto result = graph.save("/invalid/path/graph.json");
   // Returns FILE_IO_ERROR
   ```

2. **Invalid Format**: Corrupted or incompatible JSON file

   ```cpp
   auto result = graph.load("corrupted_file.json");
   // Returns INVALID_FORMAT or DESERIALIZATION_ERROR
   ```

3. **File Not Found**: Loading from non-existent file
   ```cpp
   auto result = graph.load("missing_file.json");
   // Returns FILE_IO_ERROR
   ```

## API Reference

### CinderGraph Methods

#### `save(const std::string& filepath)`

Saves the current graph to a JSON file.

**Parameters:**

- `filepath`: Path to the output file (relative or absolute)

**Returns:** `SerializationResult` with:

- `success`: Boolean indicating success/failure
- `message`: Descriptive message about the operation
- `status`: PeakStatus with detailed error code

**Example:**

```cpp
auto result = graph.save("graph.json");
```

#### `load(const std::string& filepath)`

Loads a graph from a JSON file, replacing the current graph contents.

**Parameters:**

- `filepath`: Path to the input file (relative or absolute)

**Returns:** `SerializationResult` with operation details

**Example:**

```cpp
auto result = graph.load("graph.json");
```

**Note:** Loading a graph clears all existing vertices and edges before loading the new data.

## Performance Considerations

- **File Size**: JSON format is human-readable but larger than binary formats. For very large graphs (millions of vertices/edges), file sizes can be significant.
- **Serialization Speed**: Serialization is a linear operation O(V + E) where V is vertices and E is edges.
- **Memory Usage**: Loading creates temporary data structures during deserialization.

## Future Enhancements

Planned future improvements include:

- Binary serialization format for smaller file sizes and faster I/O
- XML format support
- Streaming serialization for extremely large graphs
- Partial graph serialization (save/load subgraphs)
- Compression support

## Best Practices

1. **Use Descriptive Filenames**: Include graph type or purpose in filename

   ```cpp
   graph.save("social_network_2024.json");
   ```

2. **Check Return Values**: Always verify serialization succeeded

   ```cpp
   auto result = graph.save("graph.json");
   if (!result.success) {
       // Handle error
   }
   ```

3. **Backup Important Graphs**: Keep copies of critical graph data

   ```cpp
   graph.save("graph_backup_v1.json");
   // Make changes
   graph.save("graph_backup_v2.json");
   ```

4. **Version Control**: Consider using version numbers in filenames

   ```cpp
   graph.save("graph_v1.0.json");
   ```

5. **Test Loading**: Verify loaded graphs match expected structure
   ```cpp
   auto result = graph.load("test.json");
   assert(graph.numVertices() == expectedVertices);
   assert(graph.numEdges() == expectedEdges);
   ```

## See Also

- [Usage Guide](usage.md) - General CinderGraph usage
- [Examples](../examples/CinderGraph/serialization_example.cpp) - Complete working examples
- [API Reference](CinderGraph.md) - Full API documentation

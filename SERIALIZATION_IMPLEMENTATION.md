# Graph Serialization / Deserialization Implementation Summary

## Overview

This document summarizes the implementation of graph serialization and deserialization support in CinderPeak.

## Changes Made

### 1. Core Files Added

#### `src/StorageEngine/GraphSerialization.hpp`

- **Purpose**: Main serialization/deserialization implementation
- **Key Components**:
  - `GraphSerializer<VertexType, EdgeType>` class
  - `SerializationResult` structure for return values
  - Helper functions for type conversion (toJson/fromJson)
  - Support for primitive types, strings, CinderVertex, and CinderEdge
  - JSON-based serialization format

### 2. Core Files Modified

#### `src/CinderGraph.hpp`

- Added `#include "StorageEngine/GraphSerialization.hpp"`
- Added `save(const std::string& filepath)` method
- Added `load(const std::string& filepath)` method

#### `src/PeakStore.hpp`

- Added `getAllVertices()` method to retrieve all vertices
- Added `getAllEdges()` method to retrieve all edges
- Added `getGraphOptions()` method to get graph configuration

#### `src/StorageEngine/ErrorCodes.hpp`

- Added new error codes:
  - `FILE_IO_ERROR`
  - `SERIALIZATION_ERROR`
  - `DESERIALIZATION_ERROR`
  - `INVALID_FORMAT`
  - `UNKNOWN_ERROR`
- Added corresponding static factory methods in `PeakStatus`

#### `CMakeLists.txt`

- Integrated nlohmann/json library via FetchContent
- Linked nlohmann_json to CinderPeak interface library

### 3. Tests Added

#### `tests/unit/Serialization/test_graph_serialization.cpp`

Comprehensive test suite covering:

- Simple weighted integer graph
- String graph with double weights
- Undirected graph
- Empty graph
- Single vertex graph
- Graph with self-loops
- Invalid file paths
- Non-existent file loading
- CinderVertex serialization
- Parallel edges support
- Large graph (100 vertices, 99 edges)

### 4. Examples Added

#### `examples/CinderGraph/serialization_example.cpp`

Demonstrates:

- Creating a city transportation network
- Saving graph to JSON file
- Loading graph from JSON file
- Verifying loaded data
- Numeric graph example

#### `examples/CinderGraph/quick_serialization_test.cpp`

Quick validation tests:

- Simple integer graph
- String graph
- Empty graph
- Error handling for invalid paths

### 5. Documentation Added

#### `docs/serialization.md`

Complete user guide including:

- Feature overview
- Basic usage examples
- JSON file format specification
- Advanced examples (string graphs, custom types, configurations)
- Error handling guide
- API reference
- Performance considerations
- Best practices

#### `README.md` Updates

- Added serialization to Key Features
- Added example code showing save/load
- Updated Technology Stack to include nlohmann/json

## Feature Capabilities

### Supported Graph Types

- ✅ Directed graphs
- ✅ Undirected graphs
- ✅ Weighted graphs
- ✅ Unweighted graphs
- ✅ Graphs with self-loops
- ✅ Graphs with parallel edges

### Supported Data Types

- ✅ Primitive types (int, float, double, etc.)
- ✅ std::string
- ✅ CinderVertex
- ✅ CinderEdge
- ✅ Custom types derived from CinderVertex/CinderEdge

### Preserved Information

- ✅ All vertices
- ✅ All edges with weights
- ✅ Graph direction (directed/undirected)
- ✅ Self-loops configuration
- ✅ Parallel edges configuration
- ✅ Weighted/unweighted status

## JSON Schema

```json
{
  "graph_metadata": {
    "version": "1.0",
    "directed": true/false,
    "undirected": true/false,
    "self_loops": true/false,
    "parallel_edges": true/false,
    "weighted": true/false,
    "vertex_count": <number>,
    "edge_count": <number>
  },
  "vertices": [<vertex_data>, ...],
  "edges": [
    {
      "source": <vertex>,
      "target": <vertex>,
      "weight": <weight> (optional, only for weighted graphs)
    },
    ...
  ]
}
```

## API Usage

### Save a Graph

```cpp
CinderGraph<int, int> graph;
// ... populate graph ...
auto result = graph.save("graph.json");
if (result.success) {
    std::cout << "Saved: " << result.message << std::endl;
} else {
    std::cerr << "Error: " << result.message << std::endl;
}
```

### Load a Graph

```cpp
CinderGraph<int, int> graph;
auto result = graph.load("graph.json");
if (result.success) {
    std::cout << "Loaded: " << result.message << std::endl;
} else {
    std::cerr << "Error: " << result.message << std::endl;
}
```

## Error Handling

All serialization operations return a `SerializationResult` containing:

- `success`: Boolean flag
- `message`: Descriptive message
- `status`: PeakStatus with detailed error code

Possible error scenarios:

1. File I/O errors (invalid path, permissions)
2. Serialization errors (JSON formatting)
3. Deserialization errors (corrupt file)
4. Invalid format (missing required fields)

## Testing

### Unit Tests

- 13 comprehensive test cases in `test_graph_serialization.cpp`
- Cover normal operation and edge cases
- Verify error handling

### Example Programs

- `serialization_example.cpp`: Full featured demonstration
- `quick_serialization_test.cpp`: Quick validation

## Future Enhancements

Potential improvements:

1. Binary serialization format
2. XML format support
3. Streaming serialization for large graphs
4. Compression support
5. Partial graph serialization
6. Custom serialization callbacks

## Dependencies

- **nlohmann/json v3.11.3**: Modern C++ JSON library
  - Header-only
  - Fetched automatically via CMake FetchContent
  - No manual installation required

## Build Requirements

No additional build requirements beyond existing CinderPeak dependencies:

- C++17 or later
- CMake 3.14+
- Standard library with filesystem support

## Compatibility

The serialization feature:

- ✅ Works with all existing CinderGraph APIs
- ✅ Maintains backward compatibility
- ✅ Does not affect performance of non-serialization operations
- ✅ Thread-safe (uses existing graph thread safety mechanisms)

## Notes

1. **File Format**: JSON was chosen for version 1.0 for human readability and ease of debugging
2. **Loading Behavior**: `load()` clears existing graph before loading new data
3. **Type Safety**: Serialization preserves type information for correct deserialization
4. **Error Recovery**: All errors are handled gracefully with informative messages

## Files Created/Modified Summary

### Created (6 files)

1. `src/StorageEngine/GraphSerialization.hpp`
2. `tests/unit/Serialization/test_graph_serialization.cpp`
3. `examples/CinderGraph/serialization_example.cpp`
4. `examples/CinderGraph/quick_serialization_test.cpp`
5. `docs/serialization.md`
6. This implementation summary

### Modified (5 files)

1. `src/CinderGraph.hpp`
2. `src/PeakStore.hpp`
3. `src/StorageEngine/ErrorCodes.hpp`
4. `CMakeLists.txt`
5. `README.md`

## Total Lines of Code

- **Implementation**: ~312 lines (GraphSerialization.hpp)
- **Tests**: ~270 lines (test_graph_serialization.cpp)
- **Examples**: ~175 lines (serialization_example.cpp)
- **Documentation**: ~430 lines (serialization.md)
- **Total**: ~1,187 lines added

## Verification

To verify the implementation:

1. **Build the project**:

   ```bash
   python build.py configure --with-tests --with-examples
   python build.py build
   ```

2. **Run tests**:

   ```bash
   python build.py test
   # Or specifically:
   ./build/bin/tests/unit/Serialization/test_graph_serialization
   ```

3. **Run examples**:
   ```bash
   ./build/bin/examples/CinderGraph/serialization_example
   ./build/bin/examples/CinderGraph/quick_serialization_test
   ```

## Conclusion

The graph serialization feature is fully implemented with:

- ✅ Complete save/load functionality
- ✅ Comprehensive error handling
- ✅ Extensive test coverage
- ✅ Working examples
- ✅ Detailed documentation
- ✅ Minimal dependencies
- ✅ Clean API integration

The feature is production-ready and maintains the high quality standards of the CinderPeak project.

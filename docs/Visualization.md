# CinderPeak Visualization Engine

## Overview

The CinderPeak Visualization Engine is a modular, interactive graph visualization system built with SFML. It provides real-time rendering of graph structures with full user interaction capabilities.

## Features

- Interactive Nodes: Click, select, and drag nodes with visual feedback
- Dynamic Edges: Automatically updating connections between nodes with weight display
- Real-time Rendering: Smooth 60 FPS visualization with SFML
- Event-driven Architecture: Responsive mouse and keyboard interaction
- Modular Design: Clean separation of concerns for easy extension
- Edge Weight Visualization: Displays weights on edges when available
- Font Fallback System: Automatic font loading with multiple fallback options

## Architecture

### Core Components

1. VisualizationEngine: Main engine managing the render loop and user interaction
2. InteractiveNode: Draggable graph nodes with selection states
3. InteractiveEdge: Dynamic edges that update when nodes move
4. VisualizationTypes: Common type definitions and visual constants

### Design Decisions

- Header-only Implementation: All code in headers for easy integration
- SFML Integration: Leverages SFML's graphics and event systems
- Modern C++17: Uses smart pointers, RAII, and STL containers
- Exception Safety: Proper error handling and validation

## Building

### Prerequisites

- C++17 compatible compiler
- SFML 2.5+ (graphics, window, system components)
- CMake 3.10+

### Standalone Build

```bash
# Configure only the Visualization module
cmake -S src/Visualization -B build/vis

# Build
cmake --build build/vis

# Run demo
./build/vis/VisualizationDemo
```

## Usage

### Basic Example

```cpp
#include "Visualization/VisualizationEngine.hpp"

int main() {
    try {
        CinderPeak::Visualization::VisualizationEngine engine;
        engine.run();  // Creates demo graph and starts interaction
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
```

### Demo Graph Structure

The included demo creates a graph with:
- 5 nodes arranged in a circular layout
- Multiple weighted edges:
  - 1↔2 (weight: 1.0)
  - 2↔3 (weight: 1.5)
  - 3↔4 (weight: 2.0)
  - 4↔5 (weight: 2.5)
  - 5↔1 (weight: 3.0)
  - 1↔3 (weight: 1.8)
  - 2↔4 (weight: 2.2)
- Full drag-and-drop interaction

## Interaction

- Left Click: Select/deselect nodes
- Drag: Move selected nodes (edges update automatically)
- Close Window: Exit application

## Visual Feedback

- Normal Node: Blue circle with black outline
- Selected Node: Red/orange circle with black outline
- Dragging Node: Red outline while being dragged
- Edges: Dark gray lines connecting node centers
- Edge Weights: Displayed in black text near the center of each edge

## Integration with CinderPeak

This visualization engine integrates with CinderPeak's AdjacencyList data structure:

```cpp
#include "Visualization/VisualizationEngine.hpp"
#include "StorageEngine/AdjacencyList.hpp"

int main() {
    // Create and populate a graph
    CinderPeak::PeakStore::AdjacencyList<int, float> graph;
    
    // Add vertices
    for (int i = 1; i <= 5; ++i) {
        graph.impl_addVertex(i);
    }
    
    // Add weighted edges
    graph.impl_addEdge(1, 2, 1.0f);
    graph.impl_addEdge(2, 3, 1.5f);
    graph.impl_addEdge(3, 4, 2.0f);
    graph.impl_addEdge(4, 5, 2.5f);
    graph.impl_addEdge(5, 1, 3.0f);
    graph.impl_addEdge(1, 3, 1.8f);
    graph.impl_addEdge(2, 4, 2.2f);
    
    // Visualize the graph
    CinderPeak::Visualization::VisualizationEngine<int, float> engine(graph);
    engine.run();
    
    return 0;
}
```

## Font Support

The visualization engine includes a robust font loading system that:
- Attempts to load system fonts from common locations
- Supports multiple fallback fonts
- Gracefully handles missing fonts by falling back to a basic display
- Displays edge weights when fonts are available

## Contributing

This module follows CinderPeak's strict contribution guidelines:
- Comprehensive Doxygen documentation
- Modern C++ best practices
- Exception safety and error handling
- Consistent 4-space indentation
- Thorough testing before submission

## Dependencies

- SFML 2.5+: Graphics rendering and event handling
- C++17 STL: Smart pointers, containers, and utilities

## License

Part of the CinderPeak project. See main repository for license details.

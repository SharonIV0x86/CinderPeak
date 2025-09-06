# CinderPeak Visualization Engine

## Overview

The CinderPeak Visualization Engine is a modular, interactive graph visualization system built with SFML. It provides real-time rendering of graph structures with full user interaction capabilities.

## Features

- Interactive Nodes: Click, select, and drag nodes with visual feedback
- Dynamic Edges: Automatically updating connections between nodes
- Real-time Rendering: Smooth 60 FPS visualization with SFML
- Event-driven Architecture: Responsive mouse and keyboard interaction
- Modular Design: Clean separation of concerns for easy extension

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

The included demo creates:
- 3 interactive nodes at positions (200,200), (400,300), (600,200)
- 2 edges connecting Node1→Node2→Node3
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

## Integration with CinderPeak

This visualization engine is designed to integrate with CinderPeak's graph data structures:

```cpp
// Future integration example
GraphList<int, int> graph;
// ... populate graph ...
VisualizationEngine engine(graph);  // Visualize existing graph
```

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

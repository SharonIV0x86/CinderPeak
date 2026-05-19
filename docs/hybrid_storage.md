# CinderPeak Hybrid Storage Engine Documentation

This document describes the design mechanics, allocation patterns, and operational boundaries of CinderPeak's internal physical storage engine (PeakStore). To deliver low-latency graph traversals alongside stable insertion performance, the engine dynamically transitions between distinct matrix representations.

---

## 1. Core Dynamic Allocation Strategy

When a graph is initialized, memory management overhead must balance construction speed with analytical efficiency. CinderPeak solves this by implementing an adaptive structural tracking mechanism:

* *Sparsity Detection:* The library continuously calculates structural density metrics as new items enter memory.
* *Coordinate Mapping (COO Phase):* Initial graph construction starts within a continuous sequence of point-coordinate triplets. This layout welcomes rapid structural changes without triggering continuous array reshuffling.
* *Compact Optimization (CSR Phase):* When the structural matrix footprint passes configuration thresholds, an internal compaction routine compresses the coordinates into tight continuous rows to accelerate sequential element lookups.

---

## 2. Low-Level Component Layouts

The implementation relies on three tightly bounded vector arrays managed internally within the memory subsystem:### 2.1 The Coordinate List (COO) Array State
* *Data Fields:* Employs parallel tracks tracking row_indices, column_indices, and edge_weights.
* *Memory Trait:* Append operations remain $O(1)$ since elements settle directly into the vector terminations without forcing manual element relocations.

### 2.2 The Compressed Sparse Row (CSR) Array State
* *Data Fields:* Employs a unique rowIndexOffsets array alongside packed vertex destinations and matching weight blocks.
* *Memory Trait:* Eliminates redundant row configurations. Neighbors matching a target index $V$ occupy a predictable memory slice bounded between rowIndexOffsets[V] and rowIndexOffsets[V+1].

---

## 3. Practical Usage: Custom Storage Controls

Developers can configure custom limits to force or restrict compression behaviors depending on their client workload patterns.

### Setting Custom Compilation Limits
The code snippet below demonstrates how a user applications engineer instantiates a custom graph layout and manages memory conversion boundaries manually:

```cpp
#include "CinderPeak/CinderGraph.hpp"
#include <iostream>

int main() {
    // 1. Initialize a graph configuration specifying layout expectations
    CinderGraph<int, float> denseNetwork(GraphLayout::HYBRID);

    // 2. Populate graph paths 
    denseNetwork.addVertex(0);
    denseNetwork.addVertex(1);
    denseNetwork.addVertex(2);

    denseNetwork.addEdge(0, 1, 4.5f);
    denseNetwork.addEdge(0, 2, 1.2f);
    denseNetwork.addEdge(1, 2, 7.8f);

    // 3. Under the hood, crossing threshold profiles automatically 
    // prompts 'compressCoordinateLayout()' to freeze data blocks into CSR format.
    std::cout << "Hybrid allocation threshold checked successfully." << std::endl;
    return 0;
}## Step 3: Save and Verify
1. After pasting that into docs/hybrid_storage.md, press *Ctrl + S* to save the file.
2. Press *Ctrl + Shift + V* to check your visual markdown layout on screen to ensure the diagrams and tables render properly.

Once this is done, you will have successfully updated your two main documentation files! Let me know when it's saved, and we can discuss the next file on your checklist or show you how to securely bundle these updates using Git.
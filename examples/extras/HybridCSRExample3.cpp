#include <iostream>
#include <vector>
#include <map>
#include "CinderPeak.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore; // Assuming HybridCSR_COO is in this namespace

int main() {
    std::cout << "--- HybridCSR_COO Example 3: Disconnected Components and Self-Loops (if supported) ---" << std::endl;

    // Dummy Adjacency List:
    // 10 -> (11, 1), (10, 2) // Self-loop at 10
    // 11 -> (12, 3)
    // 20 -> (21, 5) // Disconnected component
    // 30 -> (30, 8) // Another self-loop
    std::map<int, std::vector<std::pair<int, int>>> adjList;
    adjList[10].push_back({11, 1});
    adjList[10].push_back({10, 2}); // Self-loop
    adjList[11].push_back({12, 3});
    adjList[20].push_back({21, 5});
    adjList[30].push_back({30, 8}); // Self-loop

    HybridCSR_COO<int, int> hybridStorage;
    hybridStorage.populateFromAdjList(adjList);

    std::cout << "Graph populated with disconnected components and self-loops." << std::endl;
    std::cout << "Successful population demonstrates handling of these graph structures." << std::endl;

    // Potentially add visualization call if HybridCSR_COO exposes it or via PeakStore,
    // though the request is specifically for standalone usage of populateFromAdjList.
    // hybridStorage.visualize(); // If such a method exists directly on HybridCSR_COO
    // The existing GraphMatrix and GraphList docs mention visualize().
    // If HybridCSR_COO is a backend, it might not have a direct visualize()
    // unless it implements a common interface like `StorageInterface`.

    return 0;
}
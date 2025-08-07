#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace CinderPeak::PeakStore;

int main() {
    std::cout << "--- HybridCSR_COO Example 1: Basic Population and Query ---" << std::endl;

    // Create a dummy adjacency list (vertex -> vector of (neighbor, weight))
    std::unordered_map<int, std::vector<std::pair<int, int>>> adjList = {
        {1, {{2, 10}, {3, 20}}},
        {2, {{3, 30}}},
        {3, {{1, 40}}}
    };

    // Instantiate HybridCSR_COO storage format
    HybridCSR_COO<int, int> hybridGraph;

    // Populate the storage from the adjacency list
    hybridGraph.populateFromAdjList(adjList);

    std::cout << "HybridCSR_COO populated with simple graph." << std::endl;

    // Demonstrate a query using an impl function
    int src = 1;
    int dest = 2;
    auto [edge_data, status] = hybridGraph.impl_getEdge(src, dest);

    if (status.isOK()) {
        std::cout << "Edge from " << src << " to " << dest << " exists with weight: " << edge_data << std::endl;
    } else {
        std::cout << "Edge from " << src << " to " << dest << " not found." << std::endl;
        std::cout << "Error: " << status.message() << std::endl;
    }
    
    // Demonstrate a non-existent edge
    src = 1;
    dest = 4;
    auto [edge_data_non, status_non] = hybridGraph.impl_getEdge(src, dest);

    if (!status_non.isOK()) {
        std::cout << "Correctly failed to find edge from " << src << " to " << dest << std::endl;
        std::cout << "Error: " << status_non.message() << std::endl;
    }

    return 0;
}

#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace CinderPeak::PeakStore;

int main() {
    std::cout << "--- HybridCSR_COO Example 2: String Vertices, Unweighted ---" << std::endl;

    // Dummy unweighted adjacency list (vertex -> vector of (neighbor, weight))
    // We use a dummy weight (0) for an unweighted graph
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> adjList = {
        {"A", {{"B", 0}, {"C", 0}}},
        {"B", {{"C", 0}}},
        {"C", {{"A", 0}}}
    };

    HybridCSR_COO<std::string, int> hybridGraph;
    hybridGraph.populateFromAdjList(adjList);

    std::cout << "HybridCSR_COO populated with string vertices (unweighted)." << std::endl;
    
    // Demonstrate a query using an impl function
    std::string src = "A";
    std::string dest = "B";
    auto [edge_data, status] = hybridGraph.impl_getEdge(src, dest);

    if (status.isOK()) {
        std::cout << "Edge from " << src << " to " << dest << " exists." << std::endl;
    } else {
        std::cout << "Edge from " << src << " to " << dest << " not found." << std::endl;
        std::cout << "Error: " << status.message() << std::endl;
    }

    return 0;
}

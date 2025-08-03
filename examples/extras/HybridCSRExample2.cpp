#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "CinderPeak.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore; // Assuming HybridCSR_COO is in this namespace

int main() {
    std::cout << "--- HybridCSR_COO Example 2: String Vertices, Double Weights ---" << std::endl;

    // Dummy Adjacency List:
    // "A" -> ("B", 1.5), ("C", 2.0)
    // "B" -> ("D", 3.7)
    // "C" -> ("D", 0.9), ("A", 4.1)
    std::map<std::string, std::vector<std::pair<std::string, double>>> adjList;
    adjList["A"].push_back({"B", 1.5});
    adjList["A"].push_back({"C", 2.0});
    adjList["B"].push_back({"D", 3.7});
    adjList["C"].push_back({"D", 0.9});
    adjList["C"].push_back({"A", 4.1});

    HybridCSR_COO<std::string, double> hybridStorage;
    hybridStorage.populateFromAdjList(adjList);

    std::cout << "Graph populated with string vertices and double weights." << std::endl;
    // Further operations/queries would go here, similar to Example 1,
    // once a public query API for HybridCSR_COO is known or implemented.
    std::cout << "Successfully populated a graph with string vertices and double weights." << std::endl;

    return 0;
}
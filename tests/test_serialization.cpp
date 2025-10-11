#include <iostream>
#include "CinderPeak.hpp"

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

int main() {
    // Setup graph options with valid enum members
    GraphCreationOptions opts({ GraphCreationOptions::Directed, GraphCreationOptions::SelfLoops });

    // Create a GraphList and add some data
    GraphList<int, double> graph(opts);
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 3.14);

    // Save graph to file
    if (graph.saveToFile("test_graph.txt")) {
        std::cout << "Graph saved successfully.\n";
    }

    // Load graph into a new instance
    GraphList<int, double> graphLoaded(opts);
    if (graphLoaded.loadFromFile("test_graph.txt")) {
        std::cout << "Graph loaded successfully.\n";
    }

    // Print graph statistics
    std::cout << graphLoaded.getGraphStatistics() << std::endl;

    return 0;
}

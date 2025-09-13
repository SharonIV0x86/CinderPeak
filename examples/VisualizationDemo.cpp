#include "Visualization/VisualizationEngine.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <memory>
#include <iostream>
#include <vector>

/**
 * @brief Demo application showing how to use the VisualizationEngine with AdjacencyList
 * 
 * This demo creates a sample graph with 5 nodes and several edges,
 * then visualizes it using the VisualizationEngine.
 */
int main() {
    try {
        // Create a graph
        using Graph = CinderPeak::PeakStore::AdjacencyList<int, float>;
        Graph graph;
        
        // Add some vertices
        for (int i = 1; i <= 5; ++i) {
            graph.impl_addVertex(i);
        }
        
        // Add some edges
        graph.impl_addEdge(1, 2, 1.0f);
        graph.impl_addEdge(2, 3, 1.5f);
        graph.impl_addEdge(3, 4, 2.0f);
        graph.impl_addEdge(4, 5, 2.5f);
        graph.impl_addEdge(5, 1, 3.0f);
        graph.impl_addEdge(1, 3, 1.8f);
        graph.impl_addEdge(2, 4, 2.2f);
        
        // Create and run the visualization engine with our graph
        CinderPeak::Visualization::VisualizationEngine<int, float> engine(graph);
        engine.run();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

#include "CinderGraph.hpp"
#include <iostream>

using namespace CinderPeak;

int main()
{
    std::cout << "=== CinderPeak Graph Serialization Example ===" << std::endl;
    std::cout << std::endl;

    // Create a directed, weighted graph
    GraphCreationOptions options({GraphCreationOptions::Directed,
                                  GraphCreationOptions::SelfLoops});
    CinderGraph<std::string, int> cityGraph(options);

    std::cout << "1. Creating a city transportation graph..." << std::endl;

    // Add cities (vertices)
    cityGraph.addVertex("New York");
    cityGraph.addVertex("Boston");
    cityGraph.addVertex("Philadelphia");
    cityGraph.addVertex("Washington DC");
    cityGraph.addVertex("Miami");

    // Add routes (edges) with distances
    cityGraph.addEdge("New York", "Boston", 215);
    cityGraph.addEdge("New York", "Philadelphia", 95);
    cityGraph.addEdge("Philadelphia", "Washington DC", 140);
    cityGraph.addEdge("Washington DC", "Miami", 1050);
    cityGraph.addEdge("New York", "Washington DC", 225);
    cityGraph.addEdge("Boston", "Philadelphia", 310);

    std::cout << "   Graph created with " << cityGraph.numVertices()
              << " cities and " << cityGraph.numEdges() << " routes."
              << std::endl;
    std::cout << std::endl;

    // Display some edge information
    std::cout << "2. Sample routes:" << std::endl;
    auto [dist1, found1] = cityGraph.getEdge("New York", "Boston");
    if (found1 && dist1)
    {
        std::cout << "   New York -> Boston: " << *dist1 << " miles" << std::endl;
    }

    auto [dist2, found2] = cityGraph.getEdge("Philadelphia", "Washington DC");
    if (found2 && dist2)
    {
        std::cout << "   Philadelphia -> Washington DC: " << *dist2 << " miles"
                  << std::endl;
    }

    auto [dist3, found3] = cityGraph.getEdge("Washington DC", "Miami");
    if (found3 && dist3)
    {
        std::cout << "   Washington DC -> Miami: " << *dist3 << " miles"
                  << std::endl;
    }
    std::cout << std::endl;

    // Save the graph to a file
    std::string filepath = "city_transportation_graph.json";
    std::cout << "3. Saving graph to file: " << filepath << std::endl;

    auto save_result = cityGraph.save(filepath);
    if (save_result.success)
    {
        std::cout << "   ✓ " << save_result.message << std::endl;
    }
    else
    {
        std::cerr << "   ✗ Save failed: " << save_result.message << std::endl;
        return 1;
    }
    std::cout << std::endl;

    // Create a new empty graph
    std::cout << "4. Creating a new empty graph..." << std::endl;
    CinderGraph<std::string, int> loadedGraph;
    std::cout << "   New graph has " << loadedGraph.numVertices() << " vertices."
              << std::endl;
    std::cout << std::endl;

    // Load the graph from the file
    std::cout << "5. Loading graph from file: " << filepath << std::endl;
    auto load_result = loadedGraph.load(filepath);

    if (load_result.success)
    {
        std::cout << "   ✓ " << load_result.message << std::endl;
    }
    else
    {
        std::cerr << "   ✗ Load failed: " << load_result.message << std::endl;
        return 1;
    }
    std::cout << std::endl;

    // Verify the loaded graph
    std::cout << "6. Verifying loaded graph..." << std::endl;
    std::cout << "   Loaded graph has " << loadedGraph.numVertices()
              << " cities and " << loadedGraph.numEdges() << " routes."
              << std::endl;

    // Check if vertices exist
    std::cout << "   Checking vertices:" << std::endl;
    std::cout << "     - New York: "
              << (loadedGraph.hasVertex("New York") ? "✓" : "✗") << std::endl;
    std::cout << "     - Boston: "
              << (loadedGraph.hasVertex("Boston") ? "✓" : "✗") << std::endl;
    std::cout << "     - Miami: " << (loadedGraph.hasVertex("Miami") ? "✓" : "✗")
              << std::endl;

    // Verify edges
    std::cout << "   Checking edges:" << std::endl;
    auto [v_dist1, v_found1] = loadedGraph.getEdge("New York", "Boston");
    if (v_found1 && v_dist1)
    {
        std::cout << "     - New York -> Boston: " << *v_dist1 << " miles ✓"
                  << std::endl;
    }

    auto [v_dist2, v_found2] =
        loadedGraph.getEdge("Philadelphia", "Washington DC");
    if (v_found2 && v_dist2)
    {
        std::cout << "     - Philadelphia -> Washington DC: " << *v_dist2
                  << " miles ✓" << std::endl;
    }
    std::cout << std::endl;

    // Example with numeric graph
    std::cout << "=== Example with Numeric Graph ===" << std::endl;
    std::cout << std::endl;

    CinderGraph<int, double> numericGraph;

    std::cout << "7. Creating a numeric graph (nodes 0-4)..." << std::endl;
    for (int i = 0; i < 5; ++i)
    {
        numericGraph.addVertex(i);
    }

    numericGraph.addEdge(0, 1, 1.5);
    numericGraph.addEdge(1, 2, 2.3);
    numericGraph.addEdge(2, 3, 3.7);
    numericGraph.addEdge(3, 4, 4.2);
    numericGraph.addEdge(0, 4, 8.9);

    std::cout << "   Graph created with " << numericGraph.numVertices()
              << " vertices and " << numericGraph.numEdges() << " edges."
              << std::endl;
    std::cout << std::endl;

    // Save numeric graph
    std::string numeric_filepath = "numeric_graph.json";
    std::cout << "8. Saving numeric graph to: " << numeric_filepath << std::endl;

    auto numeric_save = numericGraph.save(numeric_filepath);
    if (numeric_save.success)
    {
        std::cout << "   ✓ " << numeric_save.message << std::endl;
    }
    else
    {
        std::cerr << "   ✗ Save failed: " << numeric_save.message << std::endl;
    }
    std::cout << std::endl;

    // Load numeric graph
    std::cout << "9. Loading numeric graph from file..." << std::endl;
    CinderGraph<int, double> loadedNumericGraph;

    auto numeric_load = loadedNumericGraph.load(numeric_filepath);
    if (numeric_load.success)
    {
        std::cout << "   ✓ " << numeric_load.message << std::endl;
        std::cout << "   Loaded graph has " << loadedNumericGraph.numVertices()
                  << " vertices and " << loadedNumericGraph.numEdges() << " edges."
                  << std::endl;
    }
    else
    {
        std::cerr << "   ✗ Load failed: " << numeric_load.message << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== Serialization Example Complete ===" << std::endl;

    return 0;
}

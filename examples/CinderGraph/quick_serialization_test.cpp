#include "CinderGraph.hpp"
#include <iostream>
#include <cassert>

using namespace CinderPeak;

int main()
{
    std::cout << "Testing Graph Serialization Feature..." << std::endl;
    std::cout << "========================================" << std::endl;

    // Test 1: Simple integer graph
    std::cout << "\n[Test 1] Simple Integer Graph..." << std::endl;
    {
        CinderGraph<int, int> graph;
        graph.addVertex(1);
        graph.addVertex(2);
        graph.addVertex(3);
        graph.addEdge(1, 2, 10);
        graph.addEdge(2, 3, 20);

        auto save_result = graph.save("test_simple.json");
        std::cout << "  Save: " << (save_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;
        if (!save_result.success)
        {
            std::cout << "    Error: " << save_result.message << std::endl;
        }

        CinderGraph<int, int> loaded;
        auto load_result = loaded.load("test_simple.json");
        std::cout << "  Load: " << (load_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;
        if (!load_result.success)
        {
            std::cout << "    Error: " << load_result.message << std::endl;
        }

        std::cout << "  Verify vertices: " << (loaded.numVertices() == 3 ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "  Verify edges: " << (loaded.numEdges() == 2 ? "✓ PASS" : "✗ FAIL") << std::endl;

        auto [weight, found] = loaded.getEdge(1, 2);
        std::cout << "  Verify edge weight: " << (found && weight && *weight == 10 ? "✓ PASS" : "✗ FAIL") << std::endl;
    }

    // Test 2: String graph
    std::cout << "\n[Test 2] String Graph..." << std::endl;
    {
        CinderGraph<std::string, double> graph;
        graph.addVertex("A");
        graph.addVertex("B");
        graph.addVertex("C");
        graph.addEdge("A", "B", 1.5);
        graph.addEdge("B", "C", 2.5);

        auto save_result = graph.save("test_string.json");
        std::cout << "  Save: " << (save_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;

        CinderGraph<std::string, double> loaded;
        auto load_result = loaded.load("test_string.json");
        std::cout << "  Load: " << (load_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;

        std::cout << "  Verify vertices: " << (loaded.numVertices() == 3 ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "  Verify edges: " << (loaded.numEdges() == 2 ? "✓ PASS" : "✗ FAIL") << std::endl;
    }

    // Test 3: Empty graph
    std::cout << "\n[Test 3] Empty Graph..." << std::endl;
    {
        CinderGraph<int, int> graph;

        auto save_result = graph.save("test_empty.json");
        std::cout << "  Save: " << (save_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;

        CinderGraph<int, int> loaded;
        auto load_result = loaded.load("test_empty.json");
        std::cout << "  Load: " << (load_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;

        std::cout << "  Verify empty: " << (loaded.numVertices() == 0 && loaded.numEdges() == 0 ? "✓ PASS" : "✗ FAIL") << std::endl;
    }

    // Test 4: Invalid file path (should fail gracefully)
    std::cout << "\n[Test 4] Invalid File Path (Expected Failure)..." << std::endl;
    {
        CinderGraph<int, int> graph;
        graph.addVertex(1);

        auto save_result = graph.save("/invalid/path/graph.json");
        std::cout << "  Save failure handled: " << (!save_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;
    }

    // Test 5: Load non-existent file (should fail gracefully)
    std::cout << "\n[Test 5] Load Non-existent File (Expected Failure)..." << std::endl;
    {
        CinderGraph<int, int> graph;

        auto load_result = graph.load("nonexistent_file.json");
        std::cout << "  Load failure handled: " << (!load_result.success ? "✓ PASS" : "✗ FAIL") << std::endl;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "All basic tests completed!" << std::endl;
    std::cout << "Check output above for any failures." << std::endl;

    return 0;
}

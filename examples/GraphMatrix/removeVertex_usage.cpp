#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // 1. Basic removal
        cout << "--- Scenario 1: Basic Vertex Removal ---" << endl;
        GraphMatrix<int, double> g1;

        g1.addVertex(1);
        g1.addVertex(2);
        g1.addVertex(3);
        g1.addVertex(4);
        g1.addEdge(1, 2, 10.5);
        g1.addEdge(2, 3, 20.5);
        g1.addEdge(3, 4, 30.5);
        g1.addEdge(1, 4, 40.5);

        cout << "Initial graph: vertices 1, 2, 3, 4 with edges" << endl;

        bool removed = g1.removeVertex(2);
        cout << "Removed vertex 2: " << (removed ? "success" : "failed") << endl;
        cout << "Note: All edges connected to vertex 2 are also removed" << endl;

        // 2. Edge case: Remove non-existent vertex
        cout << "\n--- Scenario 2: Remove Non-existent Vertex ---" << endl;
        bool removed2 = g1.removeVertex(99);
        cout << "Try remove non-existent vertex 99: " << (removed2 ? "success" : "failed") << endl;

        // 3. Remove from weighted string graph
        cout << "\n--- Scenario 3: String Vertices ---" << endl;
        GraphMatrix<string, int> g2;
        g2.addVertex("A");
        g2.addVertex("B");
        g2.addVertex("C");
        g2.addVertex("D");

        g2.addEdge("A", "B", 100);
        g2.addEdge("B", "C", 200);
        g2.addEdge("C", "D", 300);
        g2.addEdge("A", "D", 400);

        cout << "Before removal: 4 vertices" << endl;
        bool removed3 = g2.removeVertex("B");
        cout << "Removed vertex B: " << (removed3 ? "success" : "failed") << endl;

        // 4. Remove vertex in undirected graph
        cout << "\n--- Scenario 4: Undirected Graph ---" << endl;
        GraphCreationOptions undirOpts({GraphCreationOptions::Undirected});
        GraphMatrix<int, float> g3(undirOpts);

        g3.addVertex(10);
        g3.addVertex(20);
        g3.addVertex(30);
        g3.addEdge(10, 20, 1.5f);
        g3.addEdge(20, 30, 2.5f);

        cout << "Undirected graph with 3 vertices" << endl;
        bool removed4 = g3.removeVertex(20);
        cout << "Removed central vertex 20: " << (removed4 ? "success" : "failed") << endl;

        // 5. Sequential removals
        cout << "\n--- Scenario 5: Sequential Removals ---" << endl;
        GraphMatrix<int, int> g4;
        for (int i = 1; i <= 5; i++) {
            g4.addVertex(i);
        }

        g4.addEdge(1, 2, 12);
        g4.addEdge(2, 3, 23);
        g4.addEdge(3, 4, 34);
        g4.addEdge(4, 5, 45);

        cout << "Initial: 5 vertices" << endl;
        g4.removeVertex(1);
        cout << "After removing 1" << endl;
        g4.removeVertex(3);
        cout << "After removing 3" << endl;
        g4.removeVertex(5);
        cout << "After removing 5 - remaining: 2, 4" << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

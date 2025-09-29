#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        GraphList<int, Unweighted> g;
        // Setup: Add vertices and edges
        g.addVertex(1);
        g.addVertex(2);
        g.addVertex(3);
        g.addVertex(4);
        g.addEdge(1, 2);
        g.addEdge(2, 3);
        g.addEdge(3, 4);
        g.addEdge(1, 4);

        cout << "Initial graph:" << endl;
        cout << "Vertices: 1, 2, 3, 4" << endl;
        cout << "Edges: (1,2), (2,3), (3,4), (1,4)" << endl;
        cout << "Total vertices: " << g.numVertices() << endl;
        cout << "Total edges: " << g.numEdges() << endl;

        // Remove vertex (also removes all connected edges)
        bool removed = g.removeVertex(2);
        cout << "\nRemoved vertex 2: " << (removed ? "success" : "failed") << endl;
        cout << "Vertices after removal: " << g.numVertices() << endl;
        cout << "Edges after removal: " << g.numEdges() << endl;

        // Try to remove non-existent vertex
        bool removed2 = g.removeVertex(99);
        cout << "\nTry remove non-existent vertex 99: " << (removed2 ? "success" : "failed") << endl;

        // Another example with weighted graph
        cout << "\n--- Weighted Graph Example ---" << endl;
        GraphList<string, double> g2;
        g2.addVertex("A");
        g2.addVertex("B");
        g2.addVertex("C");
        g2.addEdge("A", "B", 10.5);
        g2.addEdge("B", "C", 20.5);
        g2.addEdge("A", "C", 30.5);

        cout << "Before removal: " << g2.numVertices() << " vertices, " << g2.numEdges() << " edges" << endl;

        bool removed3 = g2.removeVertex("B");
        cout << "Removed vertex B: " << (removed3 ? "success" : "failed") << endl;
        cout << "After removal: " << g2.numVertices() << " vertices, " << g2.numEdges() << " edges" << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // Basic usage
    cout << "--- Basic Usage ---" << endl;
    GraphList<int, int> g;

    // Add vertices
    for (int i = 1; i <= 5; i++) {
      g.addVertex(i);
    }

    // Add edges
    g.addEdge(1, 2, 10);
    g.addEdge(2, 3, 20);
    g.addEdge(3, 4, 30);
    g.addEdge(4, 5, 40);
    g.addEdge(1, 5, 50);

    cout << "Before clearEdges:" << endl;
    cout << "  Vertices: " << g.numVertices() << endl;
    cout << "  Edges: " << g.numEdges() << endl;

    // Clear all edges
    g.clearEdges();

    cout << "\nAfter clearEdges:" << endl;
    cout << "  Vertices: " << g.numVertices() << endl;
    cout << "  Edges: " << g.numEdges() << endl;

    // Re-adding edges after clear
    cout << "\n--- Re-adding Edges ---" << endl;
    g.addEdge(1, 3, 100);
    g.addEdge(2, 4, 200);
    cout << "Added 2 new edges" << endl;
    cout << "New edge count: " << g.numEdges() << endl;

    // Unweighted graph example
    cout << "\n--- Unweighted Graph ---" << endl;
    GraphList<string, Unweighted> g2;
    g2.addVertex("A");
    g2.addVertex("B");
    g2.addVertex("C");
    g2.addVertex("D");

    g2.addEdge("A", "B");
    g2.addEdge("B", "C");
    g2.addEdge("C", "D");
    g2.addEdge("D", "A");

    cout << "Before clear: " << g2.numEdges() << " edges" << endl;
    g2.clearEdges();
    cout << "After clear: " << g2.numEdges() << " edges" << endl;
    cout << "Vertices still present: " << g2.numVertices() << endl;

    // Use case: Resetting graph structure
    cout << "\n--- Use Case: Graph Reset ---" << endl;
    GraphList<int, double> g3;
    for (int i = 0; i < 4; i++) {
      g3.addVertex(i);
    }

    // First configuration
    g3.addEdge(0, 1, 1.5);
    g3.addEdge(1, 2, 2.5);
    cout << "First configuration: " << g3.numEdges() << " edges" << endl;

    // Reset and create new configuration
    g3.clearEdges();
    g3.addEdge(0, 2, 3.0);
    g3.addEdge(2, 3, 4.0);
    cout << "Second configuration: " << g3.numEdges() << " edges" << endl;

    // Verify vertices unchanged
    cout << "Vertices remained constant: " << g3.numVertices() << endl;

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

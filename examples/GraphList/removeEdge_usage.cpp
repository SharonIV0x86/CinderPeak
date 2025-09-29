#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // Alternative 1: Use clearEdges() to remove all edges
        cout << "--- Alternative 1: clearEdges() ---" << endl;
        GraphList<int, int> g1;
        g1.addVertex(1);
        g1.addVertex(2);
        g1.addVertex(3);
        g1.addEdge(1, 2, 10);
        g1.addEdge(2, 3, 20);

        cout << "Before clearEdges: " << g1.numEdges() << " edges" << endl;
        g1.clearEdges();
        cout << "After clearEdges: " << g1.numEdges() << " edges" << endl;
        cout << "Vertices remain: " << g1.numVertices() << " vertices" << endl;

        // Alternative 2: Remove vertex (removes all connected edges)
        cout << "\n--- Alternative 2: removeVertex() ---" << endl;
        GraphList<int, Unweighted> g2;
        g2.addVertex(1);
        g2.addVertex(2);
        g2.addVertex(3);
        g2.addEdge(1, 2);
        g2.addEdge(2, 3);
        g2.addEdge(1, 3);

        cout << "Before removing vertex 2: " << g2.numEdges() << " edges" << endl;
        g2.removeVertex(2);  // Removes edges (1,2) and (2,3)
        cout << "After removing vertex 2: " << g2.numEdges() << " edges" << endl;

        // Alternative 3: Manual workaround (if you have access to internals)
        cout << "\n--- Alternative 3: Workaround ---" << endl;
        cout << "To remove a specific edge without removing vertices:" << endl;
        cout << "1. Store all edges you want to keep" << endl;
        cout << "2. Call clearEdges()" << endl;
        cout << "3. Re-add only the edges you want to keep" << endl;

        GraphList<string, double> g3;
        g3.addVertex("A");
        g3.addVertex("B");
        g3.addVertex("C");
        g3.addEdge("A", "B", 1.5);
        g3.addEdge("B", "C", 2.5);
        g3.addEdge("A", "C", 3.5);

        cout << "\nOriginal edges: " << g3.numEdges() << endl;

        // To remove edge (B,C), clear all and re-add others
        g3.clearEdges();
        g3.addEdge("A", "B", 1.5);
        g3.addEdge("A", "C", 3.5);

        cout << "After removing edge (B,C): " << g3.numEdges() << " edges" << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

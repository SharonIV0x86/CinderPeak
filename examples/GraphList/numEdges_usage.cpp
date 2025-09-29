#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        GraphList<int, int> g;

        // Setup vertices
        g.addVertex(1);
        g.addVertex(2);
        g.addVertex(3);
        g.addVertex(4);

        cout << "Initial edge count: " << g.numEdges() << endl;

        // Add edges
        g.addEdge(1, 2, 100);
        cout << "After adding edge (1,2): " << g.numEdges() << endl;

        g.addEdge(2, 3, 200);
        cout << "After adding edge (2,3): " << g.numEdges() << endl;

        g.addEdge(3, 4, 300);
        g.addEdge(4, 1, 400);
        cout << "After adding 2 more edges: " << g.numEdges() << endl;

        // Clear edges
        cout << "\n--- Testing clearEdges ---" << endl;
        g.clearEdges();
        cout << "After clearEdges: " << g.numEdges() << endl;

        // Re-add edges
        g.addEdge(1, 3, 500);
        g.addEdge(2, 4, 600);
        cout << "After re-adding 2 edges: " << g.numEdges() << endl;

        // Unweighted graph
        cout << "\n--- Unweighted Graph ---" << endl;
        GraphList<string, Unweighted> g2;

        g2.addVertex("A");
        g2.addVertex("B");
        g2.addVertex("C");
        g2.addVertex("D");

        g2.addEdge("A", "B");
        g2.addEdge("B", "C");
        g2.addEdge("C", "D");

        cout << "Unweighted edges: " << g2.numEdges() << endl;

        // Building graph progressively
        cout << "\n--- Progressive Graph Building ---" << endl;
        GraphList<int, double> g3;

        for (int i = 0; i < 5; i++) {
            g3.addVertex(i);
        }

        for (int i = 0; i < 4; i++) {
            g3.addEdge(i, i + 1, i * 1.5);
            cout << "Edges after connecting vertex " << i << " to " << (i+1)
                 << ": " << g3.numEdges() << endl;
        }

        // Parallel edges (if allowed)
        cout << "\n--- Parallel Edges ---" << endl;
        GraphCreationOptions opts({GraphCreationOptions::Directed,
                                   GraphCreationOptions::ParallelEdges});
        GraphList<int, int> g4(opts);

        g4.addVertex(10);
        g4.addVertex(20);

        g4.addEdge(10, 20, 1);
        cout << "After first edge: " << g4.numEdges() << endl;

        g4.addEdge(10, 20, 2);
        cout << "After parallel edge: " << g4.numEdges() << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // String vertices
        GraphList<string, Unweighted> g;
        g.addVertex("Alice");
        g.addVertex("Bob");
        g.addVertex("Charlie");

        // Check existing vertices
        cout << "Has Alice: " << (g.hasVertex("Alice") ? "yes" : "no") << endl;
        cout << "Has Bob: " << (g.hasVertex("Bob") ? "yes" : "no") << endl;
        cout << "Has Charlie: " << (g.hasVertex("Charlie") ? "yes" : "no") << endl;

        // Check non-existent vertex
        cout << "Has David: " << (g.hasVertex("David") ? "yes" : "no") << endl;

        // Conditional operations based on vertex existence
        cout << "\n--- Conditional Operations ---" << endl;
        if (g.hasVertex("Alice") && g.hasVertex("Bob")) {
            cout << "Both Alice and Bob exist, adding edge..." << endl;
            g.addEdge("Alice", "Bob");
        }

        if (!g.hasVertex("Eve")) {
            cout << "Eve doesn't exist, adding vertex..." << endl;
            g.addVertex("Eve");
        }

        cout << "Has Eve now: " << (g.hasVertex("Eve") ? "yes" : "no") << endl;

        // Integer vertices
        cout << "\n--- Integer Vertices ---" << endl;
        GraphList<int, double> g2;
        g2.addVertex(10);
        g2.addVertex(20);
        g2.addVertex(30);

        cout << "Has vertex 10: " << (g2.hasVertex(10) ? "yes" : "no") << endl;
        cout << "Has vertex 40: " << (g2.hasVertex(40) ? "yes" : "no") << endl;

        // Safe edge addition
        if (g2.hasVertex(10) && g2.hasVertex(20)) {
            g2.addEdge(10, 20, 5.5);
            cout << "Safely added edge (10, 20)" << endl;
        }

        // Avoid errors
        if (g2.hasVertex(10) && g2.hasVertex(999)) {
            g2.addEdge(10, 999, 1.0);
        } else {
            cout << "Cannot add edge (10, 999) - vertex 999 doesn't exist" << endl;
        }

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

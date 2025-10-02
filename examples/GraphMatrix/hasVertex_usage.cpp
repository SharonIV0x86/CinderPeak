#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // 1. Basic existence check
        cout << "--- Scenario 1: Basic Existence Check ---" << endl;
        GraphMatrix<int, double> g1;
        g1.addVertex(10);
        g1.addVertex(20);
        g1.addVertex(30);

        cout << "Has vertex 10: " << (g1.hasVertex(10) ? "yes" : "no") << endl;
        cout << "Has vertex 20: " << (g1.hasVertex(20) ? "yes" : "no") << endl;
        cout << "Has vertex 99: " << (g1.hasVertex(99) ? "yes" : "no") << endl;

        // 2. Safe edge addition with hasVertex
        cout << "\n--- Scenario 2: Safe Edge Addition ---" << endl;
        GraphMatrix<string, int> g2;
        g2.addVertex("Alice");
        g2.addVertex("Bob");

        if (g2.hasVertex("Alice") && g2.hasVertex("Bob")) {
            cout << "Both vertices exist, adding edge..." << endl;
            g2.addEdge("Alice", "Bob", 100);
        }

        if (g2.hasVertex("Alice") && g2.hasVertex("Charlie")) {
            g2.addEdge("Alice", "Charlie", 200);
        } else {
            cout << "Cannot add edge to Charlie - vertex doesn't exist" << endl;
        }

        // 3. After removal operations
        cout << "\n--- Scenario 3: After Removal ---" << endl;
        GraphMatrix<int, float> g3;
        g3.addVertex(1);
        g3.addVertex(2);
        g3.addVertex(3);

        cout << "Before removal - Has vertex 2: " << (g3.hasVertex(2) ? "yes" : "no") << endl;
        g3.removeVertex(2);
        cout << "After removal - Has vertex 2: " << (g3.hasVertex(2) ? "yes" : "no") << endl;

        // 4. Directed vs Undirected graph
        cout << "\n--- Scenario 4: Different Graph Types ---" << endl;
        GraphCreationOptions dirOpts({GraphCreationOptions::Directed});
        GraphMatrix<int, int> g4(dirOpts);

        g4.addVertex(100);
        g4.addVertex(200);

        cout << "Directed graph - Has 100: " << (g4.hasVertex(100) ? "yes" : "no") << endl;

        GraphCreationOptions undirOpts({GraphCreationOptions::Undirected});
        GraphMatrix<int, int> g5(undirOpts);

        g5.addVertex(100);
        g5.addVertex(200);

        cout << "Undirected graph - Has 100: " << (g5.hasVertex(100) ? "yes" : "no") << endl;

        // 5. Using in validation loops
        cout << "\n--- Scenario 5: Validation in Batch Operations ---" << endl;
        GraphMatrix<int, double> g6;

        vector<int> verticesToAdd = {1, 2, 3, 4, 5};
        for (int v : verticesToAdd) {
            g6.addVertex(v);
        }

        vector<pair<int, int>> edgesToAdd = {{1, 2}, {2, 3}, {3, 9}, {4, 5}};

        for (const auto& [src, dest] : edgesToAdd) {
            if (g6.hasVertex(src) && g6.hasVertex(dest)) {
                g6.addEdge(src, dest, 1.0);
                cout << "Added edge (" << src << "," << dest << ")" << endl;
            } else {
                cout << "Cannot add edge (" << src << "," << dest << ") - vertex missing" << endl;
            }
        }

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

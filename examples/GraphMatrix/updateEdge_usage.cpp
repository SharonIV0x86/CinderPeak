#include "CinderPeak.hpp"
#include <iostream>
#include <iomanip> // for setw
using namespace CinderPeak;
using namespace std;

// Utility function to print the adjacency matrix
template<typename V, typename W>
void printMatrix(GraphMatrix<V, W>& g, const string& title) {
    cout << "\n" << title << ":\n";

    auto vertices = g.getVertices();
    cout << setw(10) << "";
    for (const auto& v : vertices) {
        cout << setw(10) << v;
    }
    cout << endl;

    for (const auto& row : vertices) {
        cout << setw(10) << row;
        for (const auto& col : vertices) {
            auto [weight, found] = g.getEdge(row, col);
            if (found && weight.has_value()) {
                cout << setw(10) << weight.value();
            } else {
                cout << setw(10) << "-";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int main() {
    try {
        // 1. Basic update
        cout << "--- Scenario 1: Basic Edge Update ---" << endl;
        GraphMatrix<int, double> g1;
        g1.addVertex(1);
        g1.addVertex(2);
        g1.addEdge(1, 2, 10.0);
        printMatrix(g1, "Initial Matrix");

        g1.updateEdge(1, 2, 25.5);
        printMatrix(g1, "After Updating (1,2) to 25.5");

        // 2. Edge case: Update non-existent edge
        cout << "\n--- Scenario 2: Non-existent Edge ---" << endl;
        g1.updateEdge(1, 99, 5.0);
        g1.updateEdge(99, 100, 7.5);
        printMatrix(g1, "Matrix after trying to update non-existent edges");

        // 3. String vertices
        cout << "\n--- Scenario 3: String Vertices ---" << endl;
        GraphMatrix<string, float> g2;
        g2.addVertex("City A");
        g2.addVertex("City B");
        g2.addVertex("City C");

        g2.addEdge("City A", "City B", 100.5f);
        g2.addEdge("City B", "City C", 200.5f);
        printMatrix(g2, "Initial Matrix");

        g2.updateEdge("City A", "City B", 125.75f);
        printMatrix(g2, "After Updating City A->City B");

        // 4. Directed graph updates
        cout << "\n--- Scenario 4: Directed Graph ---" << endl;
        GraphCreationOptions dirOpts({GraphCreationOptions::Directed});
        GraphMatrix<int, int> g3(dirOpts);

        g3.addVertex(10);
        g3.addVertex(20);
        g3.addEdge(10, 20, 50);
        printMatrix(g3, "Initial Directed Matrix");

        g3.updateEdge(10, 20, 75);
        printMatrix(g3, "After Updating 10->20");

        g3.updateEdge(20, 10, 100); // Should fail
        printMatrix(g3, "After Trying to Update 20->10");

        // 5. Multiple sequential updates
        cout << "\n--- Scenario 5: Sequential Updates ---" << endl;
        GraphMatrix<int, double> g4;
        g4.addVertex(1);
        g4.addVertex(2);
        g4.addEdge(1, 2, 1.0);
        printMatrix(g4, "Initial Matrix");

        g4.updateEdge(1, 2, 2.0);
        printMatrix(g4, "After Updating to 2.0");

        g4.updateEdge(1, 2, 4.0);
        printMatrix(g4, "After Updating to 4.0");

        g4.updateEdge(1, 2, 8.0);
        printMatrix(g4, "After Updating to 8.0");

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        cout << "=== updateEdge Usage (GraphList) ===" << endl;
        cout << "Note: updateEdge only works with weighted graphs\n" << endl;
        // Basic usage
        cout << "--- Basic Usage ---" << endl;
        GraphList<int, double> g;
        g.addVertex(1);
        g.addVertex(2);
        g.addVertex(3);

        // Add initial edges
        g.addEdge(1, 2, 10.0);
        g.addEdge(2, 3, 15.0);

        cout << "Initial edge (1,2) with weight 10.0" << endl;

        // Get initial weight
        auto [initialWeight, found1] = g.getEdge(1, 2);
        if (found1 && initialWeight.has_value()) {
            cout << "Current weight of edge (1,2): " << initialWeight.value() << endl;
        }

        // Update edge weight
        auto [newWeight, updated] = g.updateEdge(1, 2, 25.5);
        cout << "\nUpdated edge (1,2) to weight " << newWeight << ": "
             << (updated ? "success" : "failed") << endl;

        // Verify update
        auto [verifyWeight, found2] = g.getEdge(1, 2);
        if (found2 && verifyWeight.has_value()) {
            cout << "Verified weight of edge (1,2): " << verifyWeight.value() << endl;
        }

        // Update another edge
        auto [w2, u2] = g.updateEdge(2, 3, 100.0);
        cout << "\nUpdated edge (2,3) to weight " << w2 << ": "
             << (u2 ? "success" : "failed") << endl;

        // Try to update non-existent edge
        cout << "\n--- Error Cases ---" << endl;
        auto [w3, u3] = g.updateEdge(1, 99, 5.0);
        cout << "Update non-existent edge (1,99): " << (u3 ? "success" : "failed") << endl;

        // String vertices
        cout << "\n--- String Vertices ---" << endl;
        GraphList<string, float> g2;
        g2.addVertex("City A");
        g2.addVertex("City B");
        g2.addEdge("City A", "City B", 100.5f);

        cout << "Initial distance: 100.5" << endl;
        auto [newDist, distUpdated] = g2.updateEdge("City A", "City B", 125.75f);
        cout << "Updated distance to " << newDist << ": "
             << (distUpdated ? "success" : "failed") << endl;

        // Multiple updates
        cout << "\n--- Multiple Updates ---" << endl;
        GraphList<int, int> g3;
        g3.addVertex(10);
        g3.addVertex(20);
        g3.addEdge(10, 20, 5);

        cout << "Initial weight: 5" << endl;
        g3.updateEdge(10, 20, 10);
        cout << "Updated to: 10" << endl;
        g3.updateEdge(10, 20, 15);
        cout << "Updated to: 15" << endl;
        g3.updateEdge(10, 20, 20);
        cout << "Updated to: 20" << endl;

        auto [finalWeight, finalFound] = g3.getEdge(10, 20);
        if (finalFound && finalWeight.has_value()) {
            cout << "Final weight: " << finalWeight.value() << endl;
        }

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

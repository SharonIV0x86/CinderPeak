#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // 1. Basic edge retrieval
        cout << "--- Scenario 1: Basic Retrieval ---" << endl;
        GraphMatrix<int, double> g1;
        g1.addVertex(1);
        g1.addVertex(2);
        g1.addVertex(3);
        g1.addVertex(4);

        g1.addEdge(1, 2, 7.5);
        g1.addEdge(2, 3, 12.0);
        g1.addEdge(3, 4, 5.25);

        auto [weight1, found1] = g1.getEdge(1, 2);
        if (found1 && weight1.has_value()) {
            cout << "Edge (1,2) weight: " << weight1.value() << endl;
        }

        auto [weight2, found2] = g1.getEdge(2, 3);
        if (found2 && weight2.has_value()) {
            cout << "Edge (2,3) weight: " << weight2.value() << endl;
        }

        // 2. Edge case: Non-existent edge
        cout << "\n--- Scenario 2: Non-existent Edges ---" << endl;
        auto [weight3, found3] = g1.getEdge(1, 4);
        if (found3 && weight3.has_value()) {
            cout << "Edge (1,4) weight: " << weight3.value() << endl;
        } else {
            cout << "Edge (1,4) does not exist" << endl;
        }

        auto [weight4, found4] = g1.getEdge(99, 100);
        if (found4 && weight4.has_value()) {
            cout << "Edge (99,100) weight: " << weight4.value() << endl;
        } else {
            cout << "Edge (99,100) does not exist (vertices missing)" << endl;
        }

        // 3. String vertices
        cout << "\n--- Scenario 3: String Vertices ---" << endl;
        GraphMatrix<string, int> g2;
        g2.addVertex("Server1");
        g2.addVertex("Server2");
        g2.addVertex("Server3");

        g2.addEdge("Server1", "Server2", 100);
        g2.addEdge("Server2", "Server3", 50);

        auto [latency1, found5] = g2.getEdge("Server1", "Server2");
        if (found5 && latency1.has_value()) {
            cout << "Latency Server1->Server2: " << latency1.value() << "ms" << endl;
        }

        // 4. Directed vs accessing reverse edges
        cout << "\n--- Scenario 4: Directed Graph ---" << endl;
        GraphCreationOptions dirOpts({GraphCreationOptions::Directed});
        GraphMatrix<int, float> g3(dirOpts);

        g3.addVertex(10);
        g3.addVertex(20);
        g3.addEdge(10, 20, 3.14f);

        auto [w5, f5] = g3.getEdge(10, 20);
        cout << "Forward edge (10,20) exists: " << (f5 ? "yes" : "no");
        if (f5 && w5.has_value()) cout << " with weight " << w5.value();
        cout << endl;

        auto [w6, f6] = g3.getEdge(20, 10);
        cout << "Reverse edge (20,10) exists: " << (f6 ? "yes" : "no") << endl;

        // 5. Conditional logic based on edge existence
        cout << "\n--- Scenario 5: Conditional Usage ---" << endl;
        GraphMatrix<int, double> g4;
        g4.addVertex(1);
        g4.addVertex(2);
        g4.addVertex(3);
        g4.addEdge(1, 2, 15.5);
        g4.addEdge(2, 3, 8.2);

        vector<pair<int, int>> edgesToCheck = {{1, 2}, {2, 3}, {1, 3}, {3, 1}};

        for (const auto& [src, dest] : edgesToCheck) {
            auto [w, found] = g4.getEdge(src, dest);
            if (found && w.has_value()) {
                if (w.value() > 10.0) {
                    cout << "Edge (" << src << "," << dest << ") weight "
                         << w.value() << " is > 10.0" << endl;
                } else {
                    cout << "Edge (" << src << "," << dest << ") weight "
                         << w.value() << " is <= 10.0" << endl;
                }
            } else {
                cout << "Edge (" << src << "," << dest << ") does not exist" << endl;
            }
        }

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

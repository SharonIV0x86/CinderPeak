#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // Enable logging before creating graph
        cout << "--- With Logging Enabled ---" << endl;
        GraphList<int, Unweighted>::setConsoleLogging(true);

        GraphList<int, Unweighted> g1;
        g1.addVertex(1);
        g1.addVertex(2);
        g1.addEdge(1, 2);

        cout << "\n--- Disable Logging ---" << endl;
        GraphList<int, Unweighted>::setConsoleLogging(false);

        g1.addVertex(3);
        g1.addEdge(2, 3);
        cout << "Operations completed silently (no debug output)" << endl;

        // Weighted graph with logging
        cout << "\n--- Weighted Graph Logging ---" << endl;
        GraphList<int, double>::setConsoleLogging(true);

        GraphList<int, double> g2;
        g2.addVertex(10);
        g2.addVertex(20);
        g2.addEdge(10, 20, 5.5);
        g2.updateEdge(10, 20, 7.5);

        GraphList<int, double>::setConsoleLogging(false);

        // Use case: Debug mode
        cout << "\n--- Debug Mode Example ---" << endl;
        bool DEBUG_MODE = true;  // Toggle this for debugging

        GraphList<int, int>::setConsoleLogging(DEBUG_MODE);

        GraphList<int, int> g3;
        g3.addVertex(100);
        g3.addVertex(200);
        g3.addEdge(100, 200, 999);

        if (DEBUG_MODE) {
            cout << "Debug information shown above" << endl;
        }

        GraphList<int, int>::setConsoleLogging(false);

        // String vertices
        cout << "\n--- String Vertices with Logging ---" << endl;
        GraphList<string, float>::setConsoleLogging(true);

        GraphList<string, float> g4;
        g4.addVertex("Node A");
        g4.addVertex("Node B");
        g4.addEdge("Node A", "Node B", 3.14f);

        GraphList<string, float>::setConsoleLogging(false);

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

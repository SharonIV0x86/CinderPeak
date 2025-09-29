#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        GraphList<int, Unweighted> g;

        // Initial count
        cout << "Initial vertex count: " << g.numVertices() << endl;

        // Add vertices
        g.addVertex(10);
        g.addVertex(20);
        g.addVertex(30);

        cout << "After adding 3 vertices: " << g.numVertices() << endl;

        // Add more vertices
        g.addVertex(40);
        g.addVertex(50);

        cout << "After adding 2 more vertices: " << g.numVertices() << endl;

        // Remove a vertex
        g.removeVertex(20);
        cout << "After removing 1 vertex: " << g.numVertices() << endl;

        // Try duplicate (won't increase count)
        g.addVertex(10);
        cout << "After trying to add duplicate: " << g.numVertices() << endl;

        // String vertices example
        cout << "\n--- String Vertices ---" << endl;
        GraphList<string, double> g2;

        vector<string> cities = {"New York", "Los Angeles", "Chicago", "Houston", "Phoenix"};
        for (const auto& city : cities) {
            g2.addVertex(city);
        }

        cout << "Number of cities: " << g2.numVertices() << endl;

        // Use in loops
        cout << "\n--- Using in Validation ---" << endl;
        GraphList<int, int> g3;

        for (int i = 0; i < 10; i++) {
            g3.addVertex(i);
            cout << "Vertices after adding " << i << ": " << g3.numVertices() << endl;
        }

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

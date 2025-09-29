#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
    try {
        // 1. Basic vertex addition
        GraphList<int, Unweighted> g1;
        auto [v1, added1] = g1.addVertex(1);
        cout << "Added vertex " << v1 << ": " << (added1 ? "success" : "failed") << endl;

        // 2. Adding multiple vertices
        auto [v2, added2] = g1.addVertex(2);
        auto [v3, added3] = g1.addVertex(3);
        cout << "Added vertices 2 and 3" << endl;
        cout << "Total vertices: " << g1.numVertices() << endl;

        // 3. Duplicate vertex handling
        auto [v4, added4] = g1.addVertex(1);
        cout << "Adding duplicate vertex 1: " << (added4 ? "success" : "failed") << endl;

        // 4. String vertices
        GraphList<string, double> g2;
        auto [vs1, addedS1] = g2.addVertex("NodeA");
        auto [vs2, addedS2] = g2.addVertex("NodeB");
        auto [vs3, addedS3] = g2.addVertex("NodeC");
        cout << "\nAdded string vertices: " << g2.numVertices() << endl;

        // 5. Adding vertices in a loop
        GraphList<int, int> g3;
        for (int i = 0; i < 5; i++) {
            auto [v, added] = g3.addVertex(i * 10);
            cout << "Added vertex " << v << ": " << (added ? "success" : "failed") << endl;
        }
        cout << "Total vertices in g3: " << g3.numVertices() << endl;

        return 0;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
}

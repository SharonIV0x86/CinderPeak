#include "CinderPeak.hpp"
#include <iostream>
#include <string>

using namespace CinderPeak;
using namespace std;

int main() {
//     try {
//         // 1. Basic operator[] usage - Reading edges in unweighted graph
//         cout << "=== Scenario 1: Unweighted Graph Edge Access ===" << endl;
//         GraphMatrix<int, Unweighted> g1;

//         g1.addVertex(1);
//         g1.addVertex(2);
//         g1.addVertex(3);

//         // Add edges
//         g1.addEdge(1, 2);
//         g1.addEdge(2, 3);

//         // Read edge using operator[] - convert to optional
//         std::optional<Unweighted> edge12 = g1[1][2];
//         cout << "Edge (1,2) exists: " << (edge12.has_value() ? "yes" : "no") << endl;

//         std::optional<Unweighted> edge13 = g1[1][3];
//         cout << "Edge (1,3) exists: " << (edge13.has_value() ? "yes" : "no") << endl;

//         // 2. Weighted graph - Reading and writing edges
//         cout << "\n=== Scenario 2: Weighted Graph Edge Access ===" << endl;
//         GraphMatrix<string, double> g2;

//         g2.addVertex("A");
//         g2.addVertex("B");
//         g2.addVertex("C");

//         // Add edge with weight
//         g2.addEdge("A", "B", 2.5);

//         // Read edge weight using operator[] - convert to optional
//         std::optional<double> edgeAB = g2["A"]["B"];
//         if (edgeAB.has_value()) {
//             cout << "Edge (A,B) weight: " << *edgeAB << endl;
//         }

//         // Check non-existent edge
//         std::optional<double> edgeAC = g2["A"]["C"];
//         cout << "Edge (A,C) exists: " << (edgeAC.has_value() ? "yes" : "no") << endl;

//         // 3. Writing edges using operator[]
//         cout << "\n=== Scenario 3: Writing Edges with operator[] ===" << endl;
//         GraphMatrix<int, int> g3;

//         g3.addVertex(10);
//         g3.addVertex(20);
//         g3.addVertex(30);

//         // Set edge weights using operator[]
//         g3[10][20] = 100;
//         g3[20][30] = 200;

//         cout << "Set edge (10,20) weight: 100" << endl;
//         cout << "Set edge (20,30) weight: 200" << endl;

//         // Verify by reading - convert to optional
//         std::optional<int> edge1020 = g3[10][20];
//         if (edge1020.has_value()) {
//             cout << "Verified edge (10,20) weight: " << *edge1020 << endl;
//         }

//         // 4. Updating existing edges
//         cout << "\n=== Scenario 4: Updating Edges ===" << endl;
//         GraphMatrix<string, float> g4;

//         g4.addVertex("X");
//         g4.addVertex("Y");

//         g4["X"]["Y"] = 5.5f;
//         cout << "Initial edge (X,Y) weight: 5.5" << endl;

//         g4["X"]["Y"] = 10.5f;
//         std::optional<float> edgeXY = g4["X"]["Y"];
//         if (edgeXY.has_value()) {
//             cout << "Updated edge (X,Y) weight: " << *edgeXY << endl;
//         }

//         return 0;
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//         return 1;
//     }
}

#include "CinderPeak.hpp"
#include <iostream>
#include <string>

using namespace CinderPeak;
using namespace std;

int main() {
//     try {
//         // 1. Enable logging before creating graph
//         cout << "=== Scenario 1: With Logging Enabled ===" << endl;
//         GraphMatrix<int, Unweighted>::setConsoleLogging(true);

//         GraphMatrix<int, Unweighted> g1;
//         g1.addVertex(1);
//         g1.addVertex(2);
//         g1.addVertex(3);
//         g1.addEdge(1, 2);
//         g1.addEdge(2, 3);

//         // 2. Disable logging
//         cout << "\n=== Scenario 2: Disable Logging ===" << endl;
//         GraphMatrix<int, Unweighted>::setConsoleLogging(false);

//         g1.addVertex(4);
//         g1.addEdge(3, 4);
//         cout << "Operations completed silently (no debug output)" << endl;

//         // 3. Weighted graph with logging
//         cout << "\n=== Scenario 3: Weighted Graph Logging ===" << endl;
//         GraphMatrix<int, double>::setConsoleLogging(true);

//         GraphMatrix<int, double> g2;
//         g2.addVertex(10);
//         g2.addVertex(20);
//         g2.addVertex(30);
//         g2.addEdge(10, 20, 5.5);
//         g2.addEdge(20, 30, 7.8);
//         g2.updateEdge(10, 20, 9.5);

//         GraphMatrix<int, double>::setConsoleLogging(false);

//         // 4. Directed graph with logging
//         cout << "\n=== Scenario 4: Directed Graph with Logging ===" << endl;
//         GraphCreationOptions directedOpts({GraphCreationOptions::Directed});
//         GraphMatrix<string, float>::setConsoleLogging(true);

//         GraphMatrix<string, float> g3(directedOpts);
//         g3.addVertex("A");
//         g3.addVertex("B");
//         g3.addVertex("C");
//         g3.addEdge("A", "B", 1.5f);
//         g3.addEdge("B", "C", 2.5f);
//         g3.addEdge("A", "C", 3.5f);

//         GraphMatrix<string, float>::setConsoleLogging(false);

//         // 5. Debug mode toggle example
//         cout << "\n=== Scenario 5: Debug Mode Toggle ===" << endl;
//         bool DEBUG_MODE = true;  // Toggle this for debugging

//         GraphMatrix<int, int>::setConsoleLogging(DEBUG_MODE);

//         GraphMatrix<int, int> g4;
//         g4.addVertex(100);
//         g4.addVertex(200);
//         g4.addVertex(300);
//         g4.addEdge(100, 200, 999);
//         g4.addEdge(200, 300, 888);

//         if (DEBUG_MODE) {
//             cout << "Debug information shown above" << endl;
//         }

//         GraphMatrix<int, int>::setConsoleLogging(false);

//         // 6. Logging during vertex removal
//         cout << "\n=== Scenario 6: Logging Vertex/Edge Removal ===" << endl;
//         GraphMatrix<int, double>::setConsoleLogging(true);

//         GraphMatrix<int, double> g5;
//         g5.addVertex(50);
//         g5.addVertex(60);
//         g5.addVertex(70);
//         g5.addEdge(50, 60, 10.5);

//         cout << "Removing vertex 60:" << endl;
//         g5.removeVertex(60);

//         GraphMatrix<int, double>::setConsoleLogging(false);

//         // 7. Self-loops with logging
//         cout << "\n=== Scenario 7: Self-loops with Logging ===" << endl;
//         GraphCreationOptions selfLoopOpts({GraphCreationOptions::Undirected,
//                                           GraphCreationOptions::SelfLoops});
//         GraphMatrix<int, int>::setConsoleLogging(true);

//         GraphMatrix<int, int> g6(selfLoopOpts);
//         g6.addVertex(111);
//         g6.addVertex(222);
//         g6.addEdge(111, 222, 100);
//         g6.addEdge(111, 111, 50);  // Self-loop
//         g6.addEdge(222, 222, 75);  // Self-loop

//         GraphMatrix<int, int>::setConsoleLogging(false);

//         // 8. Conditional logging based on operations
//         cout << "\n=== Scenario 8: Conditional Logging ===" << endl;
//         GraphMatrix<string, double> g7;

//         // Silent operation
//         GraphMatrix<string, double>::setConsoleLogging(false);
//         g7.addVertex("Node1");
//         g7.addVertex("Node2");

//         // Enable logging for specific operations
//         cout << "Enabling logging for edge operations:" << endl;
//         GraphMatrix<string, double>::setConsoleLogging(true);
//         g7.addEdge("Node1", "Node2", 4.5);

//         // Disable again
//         GraphMatrix<string, double>::setConsoleLogging(false);
//         g7.addVertex("Node3");
//         cout << "Added Node3 silently" << endl;

//         // 9. Matrix-style access with logging
//         cout << "\n=== Scenario 9: Matrix Access with Logging ===" << endl;
//         GraphMatrix<int, int>::setConsoleLogging(true);

//         GraphMatrix<int, int> g8;
//         g8.addVertex(1);
//         g8.addVertex(2);
//         g8.addVertex(3);

//         // Using operator[] to set edges
//         cout << "Setting edges using operator[]:" << endl;
//         g8[1][2] = 10;
//         g8[2][3] = 20;

//         GraphMatrix<int, int>::setConsoleLogging(false);

//         // 10. Performance testing scenario
//         cout << "\n=== Scenario 10: Performance Test (Logging Off) ===" << endl;
//         GraphMatrix<int, int>::setConsoleLogging(false);

//         GraphMatrix<int, int> g9;
//         cout << "Adding 10 vertices and edges without logging..." << endl;
//         for (int i = 1; i <= 10; i++) {
//             g9.addVertex(i * 10);
//         }
//         for (int i = 1; i < 10; i++) {
//             g9.addEdge(i * 10, (i + 1) * 10, i * 100);
//         }
//         cout << "Completed (no debug output - better performance)" << endl;

//         // Display final statistics
//         cout << "\n" << g9.getGraphStatistics() << endl;

//         return 0;
//     } catch (const exception& e) {
//         cerr << "Error: " << e.what() << endl;
//         return 1;
//     }
}

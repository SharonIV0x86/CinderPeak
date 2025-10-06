#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace std;

int main() {
  try {
    // Weighted graph
    cout << "--- Weighted Graph ---" << endl;
    CinderGraph<int, double> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addVertex(3);
    g.addVertex(4);

    g.addEdge(1, 2, 7.5);
    g.addEdge(2, 3, 12.0);
    g.addEdge(3, 4, 5.25);

    // Get existing edge
    auto [weight1, found1] = g.getEdge(1, 2);
    if (found1 && weight1.has_value()) {
      cout << "Edge (1,2) weight: " << weight1.value() << endl;
    } else {
      cout << "Edge (1,2) not found" << endl;
    }

    // Get another edge
    auto [weight2, found2] = g.getEdge(2, 3);
    if (found2 && weight2.has_value()) {
      cout << "Edge (2,3) weight: " << weight2.value() << endl;
    }

    // Try to get non-existent edge
    auto [weight3, found3] = g.getEdge(1, 4);
    cout << "Edge (1,4) found: " << (found3 ? "yes" : "no") << endl;

    // String vertices
    cout << "\n--- String Vertices ---" << endl;
    CinderGraph<string, int> g2;
    g2.addVertex("A");
    g2.addVertex("B");
    g2.addVertex("C");

    g2.addEdge("A", "B", 42);
    g2.addEdge("B", "C", 100);

    auto [w4, f4] = g2.getEdge("A", "B");
    if (f4 && w4.has_value()) {
      cout << "Edge (A,B) weight: " << w4.value() << endl;
    }

    auto [w5, f5] = g2.getEdge("B", "C");
    if (f5 && w5.has_value()) {
      cout << "Edge (B,C) weight: " << w5.value() << endl;
    }

    // Check non-existent edge
    auto [w6, f6] = g2.getEdge("A", "C");
    if (f6 && w6.has_value()) {
      cout << "Edge (A,C) weight: " << w6.value() << endl;
    } else {
      cout << "Edge (A,C) does not exist" << endl;
    }

    // Using getEdge in conditional logic
    cout << "\n--- Conditional Usage ---" << endl;
    CinderGraph<int, float> g3;
    g3.addVertex(10);
    g3.addVertex(20);
    g3.addEdge(10, 20, 15.5f);

    auto [weight, exists] = g3.getEdge(10, 20);
    if (exists && weight.has_value()) {
      if (weight.value() > 10.0f) {
        cout << "Edge weight " << weight.value() << " is greater than 10.0"
             << endl;
      }
    }

    // Checking multiple edges
    cout << "\n--- Checking Multiple Edges ---" << endl;
    CinderGraph<int, int> g4;
    g4.addVertex(1);
    g4.addVertex(2);
    g4.addVertex(3);
    g4.addEdge(1, 2, 5);
    g4.addEdge(2, 3, 10);

    vector<pair<int, int>> edgesToCheck = {{1, 2}, {2, 3}, {1, 3}};
    for (const auto &[src, dest] : edgesToCheck) {
      auto [w, f] = g4.getEdge(src, dest);
      if (f && w.has_value()) {
        cout << "Edge (" << src << "," << dest << ") has weight: " << w.value()
             << endl;
      } else {
        cout << "Edge (" << src << "," << dest << ") does not exist" << endl;
      }
    }

    return 0;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }
}

#include "CinderPeak.hpp"
#include <iostream>

using namespace CinderPeak;
using namespace CinderPeak::PeakStore;

int main() {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  GraphMatrix<int, int> graph(opts);

  // Add vertices
  for (int i = 1; i <= 8; ++i) {
    auto vResult = graph.addVertex(i);
    std::cout << "Adding vertex " << i
              << (vResult.second ? " succeeded\n" : " failed\n");
  }

  // Add cycle edges
  auto e1 = graph.addEdge(1, 2, 50);
  auto e2 = graph.addEdge(2, 3, 60);
  auto e3 = graph.addEdge(3, 4, 70);
  auto e4 = graph.addEdge(4, 5, 80);
  auto e5 = graph.addEdge(5, 6, 90);
  auto e6 = graph.addEdge(6, 7, 100);
  auto e7 = graph.addEdge(7, 8, 110);
  auto e8 = graph.addEdge(8, 1, 120);

  std::cout << "Edge add (1->2) status: " << e1.second << "\n";
  std::cout << "Edge add (8->1) status: " << e8.second << "\n";

  // Add some cross edges
  auto e9 = graph.addEdge(1, 5, 150);
  auto e10 = graph.addEdge(6, 2, 850);
  std::cout << "Edge add (1->5) status: " << e9.second << "\n";
  std::cout << "Edge add (6->2) status: " << e10.second << "\n";

  // Update an existing edge with operator[] syntax
  graph[2][5] = 45;

  // Attempt to add an invalid edge
  graph[99][99] = 45;

  // Retrieve an edge safely
  auto getRes = graph.getEdge(2, 5);
  if (getRes.second && getRes.first.has_value()) {
    std::cout << "Edge (2->5) value: " << *(getRes.first) << "\n";
  } else {
    std::cout << "Edge (2->5) not found.\n";
  }
  return 0;
}

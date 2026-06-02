#include "CinderGraph.hpp"
#include <iostream>

int main() {
  CinderPeak::CinderGraph<int, int> g(
      CinderPeak::GraphCreationOptions({CinderPeak::GraphCreationOptions::Directed}));

  g.addVertex(1);
  g.addVertex(2);
  g.addVertex(3);
  g.addEdge(1, 2, 10);
  g.addEdge(2, 3, 20);

  std::cout << "Vertices:\n";
  for (auto &v : g.vertices()) {
    std::cout << "  " << v << "\n";
  }

  std::cout << "Edges:\n";
  for (auto &[src, dest, weight] : g.edges()) {
    std::cout << "  " << src << " -> " << dest << " (weight: " << weight
              << ")\n";
  }

  return 0;
}
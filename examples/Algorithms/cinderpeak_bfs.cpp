#include "CinderGraph.hpp"
#include <iostream>

int main() {
  CinderPeak::CinderGraph<int, int> g;
  // g.addVertex(1);
  g.addVertex(2);
  g.addVertex(3);
  g.addVertex(4);
  auto result = g.bfs(1);
  if (result.isOK()) {
    for (const auto &x : result.order_) {
      std::cout << "Got: " << x << " ";
    }
  } else {
    std::cout << result._status.toString() << "\n";
  }
  std::cout << "\n";
  return 0;
}
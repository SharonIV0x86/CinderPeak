#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;

int main() {
  // Console logging via Policies
  PolicyConfiguration cfg(PolicyConfiguration::Ignore,
                          PolicyConfiguration::LogConsole);
  CinderGraph<int, int> g({GraphCreationOptions::getDefaultCreateOptions()},
                          cfg);
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 10);

  std::cout << "Logging is controlled via PolicyConfiguration.\n";
  return 0;
}

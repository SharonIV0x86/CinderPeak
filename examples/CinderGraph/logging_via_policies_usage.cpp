#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;

int main() {
  // Console logging via Policies
  PolicyConfiguration consoleCfg(PolicyConfiguration::Throw,
                                  PolicyConfiguration::LogConsole);
  CinderGraph<int, int> g1({GraphCreationOptions::Directed}, consoleCfg);
  g1.addVertex(1);
  g1.addVertex(2);
  g1.addEdge(1, 2, 10);

  // Silent (no logging)
  PolicyConfiguration silentCfg(PolicyConfiguration::Ignore,
                                 PolicyConfiguration::Silent);
  CinderGraph<int, int> g2({GraphCreationOptions::Directed}, silentCfg);
  g2.addVertex(1);
  g2.addVertex(2);
  g2.addEdge(1, 2, 10);

  // File logging
  PolicyConfiguration fileCfg(PolicyConfiguration::Ignore,
                               PolicyConfiguration::LogFile);
  CinderGraph<int, int> g3({GraphCreationOptions::Directed}, fileCfg);
  g3.addVertex(1);
  g3.addVertex(2);
  g3.addEdge(1, 2, 10);

  std::cout << "Logging is controlled via PolicyConfiguration, not setConsoleLogging.\n";
  return 0;
}

#include "CinderPeak.hpp"
using namespace CinderPeak;
using namespace std;

int main() {
  PolicyConfiguration p(PolicyConfiguration::Ignore, PolicyConfiguration::LogConsole);
  CinderGraph<int, int> g;
  g.setConsoleLogging(true);
  g.setFileLogging("local_logs.txt");
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 1);
  int x = g[1][2];
  std::cout << "\nGot edge: " << x << "\n";
}
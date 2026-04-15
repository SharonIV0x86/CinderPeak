#include "CinderPeak.hpp"
using namespace CinderPeak;
using namespace std;

int main() {
  PolicyConfiguration p(PolicyConfiguration::Ignore, PolicyConfiguration::LogConsole);
  CinderGraph<int, int> g({GraphCreationOptions::getDefaultCreateOptions()}, p);
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 1);
}
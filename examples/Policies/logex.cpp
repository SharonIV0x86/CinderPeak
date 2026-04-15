#include "CinderPeak.hpp"
using namespace CinderPeak;
using namespace std;

int main() {
  CinderGraph<int, int> g;
  g.setConsoleLogging(true);
  g.addVertex(1);
  g.addVertex(2);
  g.addEdge(1, 2, 1);
}
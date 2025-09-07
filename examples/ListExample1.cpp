#include "CinderPeak.hpp"
using namespace CinderPeak;
int main() {
  GraphCreationOptions opts(
      {GraphCreationOptions::Directed});
  GraphList<int, int> graph(opts);

  GraphList<int, int>::setConsoleLogging(true); // Enabling log display

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  graph.addVertex(4);
  graph.addVertex(5);
  graph.addEdge(1, 3, 5);
  graph.updateEdge(1, 3, 10);

  GraphCreationOptions opts1(
  {GraphCreationOptions::Directed});
  GraphList<int, Unweighted> unweighted_graph(opts1);

  unweighted_graph.addVertex(1);
  unweighted_graph.addVertex(2);
  unweighted_graph.addVertex(3);
  unweighted_graph.addVertex(4);
  unweighted_graph.addEdge(1, 2);
  Unweighted l = unweighted_graph.getEdge(1, 2);

  return 0;
}
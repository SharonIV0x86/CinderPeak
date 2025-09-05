#include "CinderPeak.hpp"
using namespace CinderPeak;
int main() {
  GraphCreationOptions opts(
      {GraphCreationOptions::Directed, GraphCreationOptions::Weighted});

  GraphList<int, int> graph(opts);
  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  graph.addVertex(4);
  graph.addVertex(5);

  graph.addEdge(1, 3, 10);
  graph.addEdge(1, 4, 9);
  graph.addEdge(4, 5, 7);
  graph.addEdge(1, 2, 6);

  GraphList<int, Unweighted> unweighted_graph(opts);
  unweighted_graph.addVertex(1);
  unweighted_graph.addVertex(2);
  unweighted_graph.addVertex(3);
  unweighted_graph.addVertex(4);
  unweighted_graph.addEdge(1, 2);
  Unweighted l = unweighted_graph.getEdge(1, 2);

  return 0;
}
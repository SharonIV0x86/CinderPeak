#include "CinderPeak.hpp"
using namespace CinderPeak;

class ListVertex : public CinderVertex {
public:
  int data;
  ListVertex(int data) : data{data} {};
  ListVertex() = default;
};

class ListEdge : public CinderEdge {
public:
  float edge_weight;
  ListEdge(float edge_weight) : edge_weight{edge_weight} {};
  ListEdge() = default;
};

int main() {
  // Graph 1
  GraphCreationOptions opts({GraphCreationOptions::Directed});
  GraphList<int, int> graph(opts);

  GraphList<int, int>::setConsoleLogging(true); // Enabling log display

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  std::cout << "Number of vertices: " << graph.numVertices()
            << "\n"; // Testing numVertices implementation

  graph.addVertex(4);
  graph.addVertex(5);
  graph.addEdge(1, 3, 5);
  graph.updateEdge(1, 3, 10);
  std::cout << "Number of vertices: " << graph.numVertices()
            << "\n"; // Testing numVertices implementation

  // Graph 2
  GraphCreationOptions opts1({GraphCreationOptions::Directed});
  GraphList<int, Unweighted> unweighted_graph(opts1);

  unweighted_graph.addVertex(1);
  unweighted_graph.addVertex(2);
  unweighted_graph.addVertex(3);
  unweighted_graph.addVertex(4);
  unweighted_graph.addEdge(1, 2);
  Unweighted l = unweighted_graph.getEdge(1, 2);
  std::cout << "Number of vertices: " << unweighted_graph.numVertices()
            << "\n"; // Testing numVertices implementation

  // Graph 3
  GraphCreationOptions options({GraphCreationOptions::Undirected});
  ListVertex lv1(1);
  ListVertex lv2(2);
  ListEdge e1(0.5f);
  ListEdge e2(0.8f);
  GraphList<ListVertex, ListEdge> listGraph(opts);
  listGraph.addVertex(lv1);
  listGraph.addVertex(lv2);

  listGraph.addEdge(lv1, lv2, e1);
  auto beforeEdge = listGraph.getEdge(lv1, lv2);
  std::cout << "Edge value before update: " << beforeEdge.edge_weight << "\n";
  std::cout << "Updaing Edge Value to 0.8\n";

  listGraph.updateEdge(lv1, lv2, e2);
  auto afterEdge = listGraph.getEdge(lv1, lv2);
  std::cout << "Edge value after the update: " << afterEdge.edge_weight << "\n";

  return 0;
}

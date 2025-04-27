#include <iostream>
#include "PeakStore.hpp"
#include "GraphMatrix.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class Vertex : public CinderVertex
{
public:
  int data;

public:
  Vertex() {}
};
class Edge : public CinderEdge
{
public:
  int dd;

public:
  Edge() {}
};
int main()
{
  GraphCreationOptions options({GraphCreationOptions::Undirected,
                                GraphCreationOptions::Weighted,
                                GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata("graph_matrix", isTypePrimitive<Vertex>(), isTypePrimitive<Edge>());

  GraphMatrix<Vertex, Edge> gm(options);

  Vertex v1;
  Vertex v2;
  Edge e;
  e.dd = 1290;
  // try
  // {
  gm.addVertex(v1);
  gm.addVertex(v1);
  gm.addVertex(v2);
  gm.addEdge(v1, v2, e);
  gm.addEdge(v1, v2, e);
  Edge edge = gm.getEdge(v1, v2);
  std::cout << "MEOW: " << edge.dd << "\n";
  // }
  // catch (const GraphException &e)
  // {
  //   std::cerr << e.what() << '\n';
  // }

  // Vertex vv(v);
  return 0;
}
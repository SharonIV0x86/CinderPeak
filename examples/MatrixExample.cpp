#include "CinderPeak.hpp"
#include <iostream>
// #include "PeakStore.hpp"
// #include "GraphMatrix.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class CustomVertex : public CinderVertex {
public:
  int data;

public:
  CustomVertex() {}
};
class CustomEdge : public CinderEdge {
public:
  int dd;

public:
  CustomEdge() {}
};
int main() {
  GraphCreationOptions options({GraphCreationOptions::Undirected,
                                GraphCreationOptions::SelfLoops});

  GraphMatrix<CustomVertex, CustomEdge> myGraph(options);

<<<<<<< HEAD
  myGraph.setConsoleLogging(false); // Disabling log display
=======
  myGraph.togglePLogging(false); // Disabling log display
>>>>>>> 9b19174 (Examples updated to test toggling functionality)

  CustomVertex v1;
  CustomVertex v2;
  CustomEdge e;
  e.dd = 1290;
  myGraph.addVertex(v1);
  myGraph.addVertex(v1);
  myGraph.addVertex(v2);
  // myGraph.addEdge(v1, v2, e);
  myGraph[v1][v2] = e;
  CustomEdge edge = myGraph.getEdge(v1, v2);
  std::cout << "Edge between v1 and v2: " << edge.dd << "\n";

  GraphMatrix<int, int> mock(options);
  mock.addVertex(1);
  mock.addVertex(2);
  mock.addEdge(1, 2, 10);
  std::cout << "Edge dete: " << mock[1][2] << "\n";

  return 0;
}
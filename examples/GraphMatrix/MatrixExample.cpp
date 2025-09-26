#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak;
using namespace CinderPeak::PeakStore;

// Custom Vertex Type
class CustomVertex : public CinderVertex {
public:
  int data;
  CustomVertex(int d = 0) : data(d) {}
};

// Custom Edge Type
class CustomEdge : public CinderEdge {
public:
  int dd;
  CustomEdge(int val = 0) : dd(val) {}
};

int main() {
  GraphCreationOptions options(
      {GraphCreationOptions::Undirected, GraphCreationOptions::SelfLoops});

  GraphMatrix<CustomVertex, CustomEdge> myGraph(options);

  GraphMatrix<CustomVertex, CustomEdge>::setConsoleLogging(false);

  CustomVertex v1(1);
  CustomVertex v2(2);
  CustomEdge e(1290);

  // Add vertices
  auto v1Result = myGraph.addVertex(v1);
  auto v2Result = myGraph.addVertex(v2);
  std::cout << "Added v1: " << v1Result.second
            << ", Added v2: " << v2Result.second << "\n";

  // Add edge
  auto edgeResult = myGraph.addEdge(v1, v2, e);
  std::cout << "Edge add status: " << edgeResult.second << "\n";

  // Retrieve edge
  auto getResult = myGraph.getEdge(v1, v2);
  if (getResult.second) {
    std::cout << "Edge between v1 and v2: " << getResult.first->dd << "\n";
  } else {
    std::cout << "No edge found between v1 and v2.\n";
  }

  // Another example with primitive types
  GraphMatrix<int, int> mock(options);
  mock.addVertex(1);
  mock.addVertex(2);

  auto mockEdgeAdd = mock.addEdge(1, 2, 10);
  std::cout << "Mock edge add status: " << mockEdgeAdd.second << "\n";

  auto mockGet = mock.getEdge(1, 2);
  if (mockGet.second) {
    std::cout << "Edge [1->2] value: " << *(mockGet.first) << "\n";
  }

  return 0;
}

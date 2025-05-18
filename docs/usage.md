# CinderPeak GraphMatix Example

```cpp
#include <iostream>
#include "PeakStore.hpp"
#include "GraphMatrix.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class CustomVertex : public CinderVertex
{
public:
  int data;

public:
  CustomVertex() {}
};
class CustomEdge : public CinderEdge
{
public:
  int dd;

public:
  CustomEdge() {}
};
int main()
{
  GraphCreationOptions options({GraphCreationOptions::Undirected,
                                GraphCreationOptions::Weighted,
                                GraphCreationOptions::SelfLoops});

  GraphMatrix<CustomVertex, CustomEdge> myGraph(options);

  CustomVertex v1;
  CustomVertex v2;
  CustomEdge e;
  e.dd = 1290;
  myGraph.addVertex(v1);
  myGraph.addVertex(v1);
  myGraph.addVertex(v2);
  myGraph.addEdge(v1, v2, e);
  myGraph.addEdge(v1, v2, e);
  CustomEdge edge = myGraph.getEdge(v1, v2);
  std::cout << "Edge between v1 and v2: " << edge.dd << "\n";
  return 0;
}
```

# Explanation
This example demonstrates how to use the GraphMatrix class in the CinderPeak library with custom vertex and edge types. We begin by defining two classes, CustomVertex and CustomEdge, which inherit from CinderVertex and CinderEdge respectively. The CustomVertex class contains an integer field data, while CustomEdge includes a field dd to store custom edge data (in this case, an integer weight).


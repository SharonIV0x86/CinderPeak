#include "CinderPeak.hpp"
using namespace CinderPeak;
int main()
{
    GraphCreationOptions opts({GraphCreationOptions::Directed,
                               GraphCreationOptions::Weighted});

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

    graph.visualize();
}
#include <iostream>
#include "PeakStore.hpp"
#include "GraphMatrix.hpp"

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;


int main(){
    GraphCreationOptions opts({GraphCreationOptions::Weighted, 
                             GraphCreationOptions::Undirected});
    GraphMatrix<int, int> graph(opts);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    graph.addVertex(4);

    graph.addEdge(1, 2, 100);
    graph.addEdge(2, 3, 200);
    graph.addEdge(3, 4, 300);
    graph.addEdge(4, 1, 400);

    graph.visualize();
}
#include <iostream>
#include "PeakStore.hpp"
#include "GraphMatrix.hpp"

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

int main() {
    GraphCreationOptions opts({GraphCreationOptions::Weighted, 
                               GraphCreationOptions::Undirected});
    GraphMatrix<int, int> graph(opts);

    for (int i = 1; i <= 8; ++i) {
        graph.addVertex(i);
    }

    graph.addEdge(1, 2, 50);
    graph.addEdge(2, 3, 60);
    graph.addEdge(3, 4, 70);
    graph.addEdge(4, 5, 80);
    graph.addEdge(5, 6, 90);
    graph.addEdge(6, 7, 100);
    graph.addEdge(7, 8, 110);
    graph.addEdge(8, 1, 120);  

    graph.addEdge(1, 5, 150);
    graph.addEdge(6, 2, 850);
    
    // graph.addEdge(2, 6, 160);
    // graph.addEdge(3, 7, 170);
    // graph.addEdge(4, 8, 180);

    graph.visualize();
}

#include "CinderPeak.hpp"
#include <iostream>
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

void Example1() {
  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      adjList = {{1, {{2, 10}, {3, 20}}}, {2, {{3, 30}}}, {3, {{1, 40}}}};

  HybridCSR_COO<int, int> graph;
  graph.populateFromAdjList(adjList);

  std::cout << "Example1 Graph:\n";
  graph.exc();
}

void Example2() {
  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      adjList2 = {{1, {{2, 10}, {3, 20}}},
                  {2, {{1, 10}}},
                  {3, {{1, 20}, {4, 50}, {5, 40}}},
                  {4, {{3, 50}}},
                  {5, {{3, 40}}}};

  HybridCSR_COO<int, int> graph;
  graph.populateFromAdjList(adjList2);

  std::cout << "Example2 Graph:\n";
  graph.exc();
}

void Example3() {
  std::unordered_map<int, std::vector<std::pair<int, int>>, VertexHasher<int>>
      adjList3 = {{1, {{2, 10}, {3, 30}}},
                  {2, {{1, 10}, {4, 20}}},
                  {3, {{1, 20}, {4, 10}}},
                  {4, {{2, 20}, {3, 10}}}};

  HybridCSR_COO<int, int> graph;
  graph.populateFromAdjList(adjList3);

  std::cout << "Example3 Graph:\n";
  graph.exc();
}

int main() {
  Example1();
  Example2();
  Example3();
  return 0;
}

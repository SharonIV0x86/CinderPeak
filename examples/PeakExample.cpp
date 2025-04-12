#include "PeakStore.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;
int main()
{
  GraphCreationOptions options({GraphCreationOptions::Directed,
                                GraphCreationOptions::Weighted,
                                GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata;
  // metadata.num_edges = 4;
  // metadata.num_vertices = 2;

  std::shared_ptr<GraphInternalMetadata> mt = std::make_shared<GraphInternalMetadata>(metadata);
  std::shared_ptr<GraphCreationOptions> opt = std::make_shared<GraphCreationOptions>(options);
  CinderPeak::PeakStore::PeakStore<int, int> store(metadata);
  std::cout << "\t---Testing AJD\n";
  std::cout << "ADJ Density: " << store.adjacency_storage->graph_metadata->density << "\n";
  std::cout << "SET Density to: 3\n";
  store.adjacency_storage->graph_metadata->density = 3;
  std::cout << "ADJ Density: " << store.adjacency_storage->graph_metadata->density << "\n";

  std::cout << "\t---Testing HYBRID\n";
  std::cout << "HYBRID Density: " << store.hybrid_storage->graph_metadata->density << "\n";
  std::cout << "SET Num Vetices: 24\n";
  store.hybrid_storage->graph_metadata->num_vertices = 24;

  std::cout << "\n---Testing PEAK\n";
  std::cout << "PEAK Num Vertices: " << store.graph_metadata->num_vertices << "\n";
  std::cout << "SET Num Parallel Edges: " << store.graph_metadata->num_parallel_edges << "\n";
  store.graph_metadata->num_parallel_edges = 91;

  std::cout << "\n---Final Test from ADJ\n";
  std::cout << "ADJ Density : " << store.adjacency_storage->graph_metadata->density << "\n";
  std::cout << "ADJ Num Vertices : " << store.adjacency_storage->graph_metadata->num_vertices << "\n";
  std::cout << "ADJ Num Edges : " << store.adjacency_storage->graph_metadata->num_edges << "\n";
  std::cout << "ADJ Num Self Loops : " << store.adjacency_storage->graph_metadata->num_self_loops << "\n";
  std::cout << "ADJ Num Parallel Edges : " << store.adjacency_storage->graph_metadata->num_parallel_edges << "\n";
  store.addVertex(1);
  store.addVertex(2);
  store.addVertex(3);
  store.addVertex(4);
  PeakStatus response = store.addEdge(1, 2, 4);
  if(response.isOK()){
    std::cout << "Edge 1->2 added successfully\n";
  }
  response = store.addEdge(1, 3, 5);
  if(response.isOK()){
    std::cout << "Edge 1->3 added successfully\n";
  }
  response = store.addEdge(1, 4, 8);
  if(response.isOK()){
    std::cout << "Edge 1->4 added successfully\n";
  }
  int egde = store.getEdge(1, 2);
  std::pair<std::vector<std::pair<int, int>>, PeakStatus> neighbors = store.getNeighbors(1);
  std::cout << "Edge between: 1 and 2: " << egde << "\n";
  std::vector<std::pair<int, int>> nn = neighbors.first;
  for(const auto&[dest, edge]: nn){
    std::cout << dest << " ";
  }
  neighbors.second = PeakStatus::OK();
  std::cout << "\n";
  return 0;

}
#include "PeakStore.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;
int main()
{
  GraphCreationOptions options({GraphCreationOptions::Directed,
                                GraphCreationOptions::Weighted,
                                GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata("graph_matrix");
  // metadata.num_edges = 4;s
  // metadata.num_vertices = 2;

  std::shared_ptr<GraphInternalMetadata> mt = std::make_shared<GraphInternalMetadata>(metadata);
  std::shared_ptr<GraphCreationOptions> opt = std::make_shared<GraphCreationOptions>(options);
  CinderPeak::PeakStore::PeakStore<int, int> store(metadata);
  store.addVertex(1);
  std::cout << store.ctx->metadata->num_edges << "\n";
  std::cout << store.ctx->metadata->num_vertices << "\n";
  
  // store.addVertex(1);
  // store.addVertex(2);
  // store.addVertex(3);
  // store.addVertex(4);
  // PeakStatus response = store.addEdge(1, 2, 4);
  // if(response.isOK()){
  //   std::cout << "Edge 1->2 added successfully\n";
  // }
  // response = store.addEdge(1, 3, 5);
  // if(response.isOK()){
  //   std::cout << "Edge 1->3 added successfully\n";
  // }
  // response = store.addEdge(1, 4, 8);
  // if(response.isOK()){
  //   std::cout << "Edge 1->4 added successfully\n";
  // }
  // int egde = store.getEdge(1, 2);
  // std::pair<std::vector<std::pair<int, int>>, PeakStatus> neighbors = store.getNeighbors(1);
  // std::cout << "Edge between: 1 and 2: " << egde << "\n";
  // std::vector<std::pair<int, int>> nn = neighbors.first;
  // for(const auto&[dest, edge]: nn){
  //   std::cout << dest << " ";
  // }
  // neighbors.second = PeakStatus::OK();
  // std::cout << "\n";
  return 0;

}
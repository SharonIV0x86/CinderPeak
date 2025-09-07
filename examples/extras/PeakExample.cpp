#include "PeakStore.hpp"
#include "Concepts.hpp"

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

int main() {
  GraphCreationOptions options({GraphCreationOptions::Directed,
                                GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata("graph_matrix", Traits::isTypePrimitive<int>(),
                                 Traits::isTypePrimitive<int>(), Traits::isGraphWeighted<int>(), !Traits::isGraphWeighted<int>());

  // Optionally create shared_ptrs if needed elsewhere, but not required for store:
  std::shared_ptr<GraphInternalMetadata> mt =
      std::make_shared<GraphInternalMetadata>(metadata);
  std::shared_ptr<GraphCreationOptions> opt =
      std::make_shared<GraphCreationOptions>(options);

  CinderPeak::PeakStore::PeakStore<int, int> store(metadata);

  store.addVertex(1);
  store.addVertex(1);

  std::cout << store.getContext()->metadata->num_edges << "\n";
  std::cout << store.getContext()->metadata->num_vertices << "\n";

  // Uncomment and use more test code as needed:
  /*
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
  auto edge = store.getEdge(1, 2);
  std::pair<std::vector<std::pair<int, int>>, PeakStatus> neighbors =
      store.getNeighbors(1);
  std::cout << "Edge between: 1 and 2: " << edge.first << "\n";
  std::vector<std::pair<int, int>> nn = neighbors.first;
  for(const auto& [dest, edge]: nn){
    std::cout << dest << " ";
  }
  neighbors.second = PeakStatus::OK();
  std::cout << "\n";
  */
  return 0;
}

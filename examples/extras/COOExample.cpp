#include <iostream>
#include "PeakStore.hpp"

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;
int main()
{
  GraphCreationOptions options({GraphCreationOptions::Directed,
                                GraphCreationOptions::Weighted,
                                GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata("graph_matrix", false, false);
  // metadata.num_edges = 4;s
  // metadata.num_vertices = 2;

  std::shared_ptr<GraphInternalMetadata> mt = std::make_shared<GraphInternalMetadata>(metadata);
  std::shared_ptr<GraphCreationOptions> opt = std::make_shared<GraphCreationOptions>(options);
  CinderPeak::PeakStore::PeakStore<int, int> store(metadata);
  store.getContext()->coordinate_list->impl_addVertex(1);
  store.getContext()->coordinate_list->impl_addVertex(2);
  store.getContext()->coordinate_list->impl_addVertex(3);
  store.getContext()->coordinate_list->impl_addVertex(4);
  
  store.getContext()->coordinate_list->impl_addEdge(1,2,1);
  store.getContext()->coordinate_list->impl_addEdge(2,3,2);
  store.getContext()->coordinate_list->impl_addEdge(3,4,3);
  store.getContext()->coordinate_list->impl_addEdge(4,1,4);

  auto resp = store.getContext()->coordinate_list->impl_getEdge(1,2);
  std::cout << resp.first << "\n";
  
  return 0;

}
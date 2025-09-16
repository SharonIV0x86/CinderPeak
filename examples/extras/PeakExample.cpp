#include "Concepts.hpp"
#include "PeakStore.hpp"
using namespace CinderPeak::PeakStore;
using namespace CinderPeak;
int main() {
  GraphCreationOptions options(
      {GraphCreationOptions::Directed, GraphCreationOptions::SelfLoops});
  GraphInternalMetadata metadata("graph_matrix", Traits::isTypePrimitive<int>(),
                                 Traits::isTypePrimitive<int>(),
                                 Traits::isGraphWeighted<int>(),
                                 !Traits::isGraphWeighted<int>());

  std::shared_ptr<GraphInternalMetadata> mt =
      std::make_shared<GraphInternalMetadata>(metadata);
  std::shared_ptr<GraphCreationOptions> opt =
      std::make_shared<GraphCreationOptions>(options);
  CinderPeak::PeakStore::PeakStore<int, int> store(metadata);
  store.addVertex(1);
  store.addVertex(1);
  std::cout << store.getContext()->metadata->num_edges << "\n";
  std::cout << store.getContext()->metadata->num_vertices << "\n";

  return 0;
}
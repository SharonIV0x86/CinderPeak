#include "StorageEngine/AdjacencyList.hpp"
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
namespace CinderPeak
{
  namespace PeakStore
  {
    const CinderPeak::GraphCreationOptions DEFAULT_GRAPH_OPTIONS(
        {CinderPeak::GraphCreationOptions::Directed,
         CinderPeak::GraphCreationOptions::SelfLoops});
    template <typename VertexType, typename EdgeType>
    class PeakStore
    {
    public:
      std::unique_ptr<HybridCSR_COO<VertexType, EdgeType>> hybrid_storage = nullptr;
      std::shared_ptr<GraphInternalMetadata> graph_metadata = nullptr;
      std::shared_ptr<GraphCreationOptions> create_options = nullptr;
      std::unique_ptr<AdjacencyList<VertexType, EdgeType>> adjacency_storage = nullptr;

      PeakStore(const GraphInternalMetadata &metadata, const GraphCreationOptions &options = DEFAULT_GRAPH_OPTIONS)
      {
        graph_metadata = std::make_shared<GraphInternalMetadata>(metadata);
        create_options = std::make_shared<GraphCreationOptions>(options);
        hybrid_storage =
            std::make_unique<HybridCSR_COO<VertexType, EdgeType>>(graph_metadata, create_options);
        adjacency_storage =
            std::make_unique<AdjacencyList<VertexType, EdgeType>>(graph_metadata, create_options);
      }
    };

  } // namespace PeakStore
} // namespace CinderPeak
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

}
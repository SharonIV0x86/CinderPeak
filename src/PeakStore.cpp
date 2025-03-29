#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/Utils.hpp"
namespace CinderPeak
{

  namespace PeakStore
  {

    class PeakStore{

    };

    
  } // namespace PeakStore
} // namespace CinderPeak

int main()
{
  CinderPeak::PeakStore::GraphCreationOptions options({
      CinderPeak::PeakStore::GraphType::Directed,
      CinderPeak::PeakStore::GraphType::Weighted,
      CinderPeak::PeakStore::GraphType::SelfLoops,
  });
  CinderPeak::PeakStore::GraphInternalMetadata metadata;
  metadata.num_edges = 4;
  metadata.num_vertices = 2;

  CinderPeak::PeakStore::HybridCSR_COO<int, int> graph(options, metadata);
  std::cout << "Has Directed: " << graph.hasOption(CinderPeak::PeakStore::GraphType::Directed) << "\n";
  std::cout << "Has Weighted: " << graph.hasOption(CinderPeak::PeakStore::GraphType::Weighted) << "\n";
  std::cout << "Has ParallelEdges: " << graph.hasOption(CinderPeak::PeakStore::GraphType::ParallelEdges) << "\n";
  std::cout << "Has SelfLoops: " << graph.hasOption(CinderPeak::PeakStore::GraphType::SelfLoops) << "\n";
}
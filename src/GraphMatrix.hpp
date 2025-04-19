#include "StorageEngine/Utils.hpp"
#include <iostream>

namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}

template <typename VertexType, typename EdgeType> class GraphMatrix {
private:
  std::unique_ptr<CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>
      peak_store;

public:
  GraphMatrix(const GraphCreationOptions &options =
                  CinderPeak::PeakStore::DEFAULT_GRAPH_OPTIONS) {
    CinderPeak::PeakStore::GraphInternalMetadata metadata("graph_matrix");
    peak_store = std::make_unique<
        CinderPeak::PeakStore::PeakStore<VertexType, EdgeType>>(metadata,
                                                                options);
  }
  void addVertex(const VertexType &src) {
    auto resp = peak_store->addVertex(src);
    if(!resp.isOK()){
      Exceptions::handle_exception_map(resp);
      return ;
    }
    
  }
};
} // namespace CinderPeak

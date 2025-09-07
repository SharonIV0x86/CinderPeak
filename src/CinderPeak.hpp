#pragma once

#include "GraphList.hpp"
#include "GraphMatrix.hpp"
#include "PeakStore.hpp"
#include "StorageEngine/Utils.hpp"
namespace CinderPeak {
namespace PeakStore {
template <typename VertexType, typename EdgeType> class PeakStore;
}
struct GraphCreationOptions {
    enum Type { Undirected, Directed };
    GraphCreationOptions(std::initializer_list<Type> t) {}
    static GraphCreationOptions getDefaultCreateOptions() { return GraphCreationOptions({Undirected}); }
};
namespace Traits {
    template<typename T> constexpr bool is_unweighted_v = false;
    template<typename T> constexpr bool is_weighted_v = true;
    template<typename T> constexpr bool isTypePrimitive() { return std::is_fundamental<T>::value; }
    template<typename T> constexpr bool isGraphWeighted() { return true; }
}

template <typename VertexType, typename EdgeType> class GraphMatrix;
template <typename VertexType, typename EdgeType> class GraphList;

} // namespace CinderPeak

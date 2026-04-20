#pragma once

#include "core/CinderGraph.hpp"
#include "algorithms/CinderPeakAlgorithms.hpp"

namespace cinderpeak {

template <typename VertexType, typename EdgeType>
using CinderGraph = CinderPeak::CinderGraph<VertexType, EdgeType>;

namespace algo {
using namespace CinderPeak::Algorithms;
}

} // namespace cinderpeak

#pragma once

#include "algorithms/CinderPeakAlgorithms.hpp"
#include "core/CinderGraph.hpp"

namespace cinderpeak {

template <typename VertexType, typename EdgeType>
using CinderGraph = CinderPeak::CinderGraph<VertexType, EdgeType>;

namespace algo {
using namespace CinderPeak::Algorithms;
}

} // namespace cinderpeak

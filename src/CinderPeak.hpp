// CinderPeak.hpp
#pragma once

// Main Graph Implementations
#include "GraphList.hpp"
#include "GraphMatrix.hpp"
#include "PeakStore.hpp"
#include "StorageEngine/Utils.hpp"
// Optionally include utility types, exceptions, and storage engine if needed
// #include "CinderExceptions.hpp"
// #include "PeakLogger.hpp"
// #include "StorageInterface.hpp"
// #include "Visualizer.hpp"

namespace CinderPeak
{
  namespace PeakStore
  {
    template <typename VertexType, typename EdgeType>
    class PeakStore;
  }

  template <typename VertexType, typename EdgeType>
  class GraphMatrix;
  template <typename VertexType, typename EdgeType>
  class GraphList;

  // class CinderGraph
  // {
  // public:
  //   static GraphCreationOptions getDefaultCreateOptions()
  //   {
  //     const GraphCreationOptions DEFAULT_GRAPH_OPTIONS(
  //         {GraphCreationOptions::Directed,
  //          GraphCreationOptions::SelfLoops});

  //     return DEFAULT_GRAPH_OPTIONS;
  //   }
  // };
} // namespace CinderPeak

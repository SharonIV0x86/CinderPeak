#pragma once
#include "CinderPeak.hpp"

namespace CinderPeak {

enum class GraphOpts : uint8_t { directed, undirected };

class ListVertex : public CinderVertex {
public:
  int data;
  ListVertex(int d = 0) : data(d) {}
  bool operator==(const ListVertex &other) const { return data == other.data; }
};

class ListEdge : public CinderEdge {
public:
  float edge_weight;
  ListEdge(float w = 0.0f) : edge_weight(w) {}
  bool operator==(const ListEdge &other) const {
    return edge_weight == other.edge_weight;
  }
};

class DummyGraph {
protected:
  GraphCreationOptions directedOpts;
  GraphCreationOptions undirectedOpts;

public:
  DummyGraph()
      : directedOpts({GraphCreationOptions::Directed}),
        undirectedOpts({GraphCreationOptions::Undirected}) {}
  // Primitive graphs
  CinderGraph<int, int>
  CreatePrimitiveWeightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<int, int>(directedOpts);
    } else {
      return CinderGraph<int, int>(undirectedOpts);
    }
  }

  CinderGraph<int, Unweighted>
  CreatePrimitiveUnweightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<int, Unweighted>(directedOpts);
    } else {
      return CinderGraph<int, Unweighted>(undirectedOpts);
    }
  }

  // String graphs
  CinderGraph<std::string, float>
  CreateStringWeightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<std::string, float>(directedOpts);
    } else {
      return CinderGraph<std::string, float>(undirectedOpts);
    }
  }

  CinderGraph<std::string, Unweighted>
  CreateStringUnweightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<std::string, Unweighted>(directedOpts);
    } else {
      return CinderGraph<std::string, Unweighted>(undirectedOpts);
    }
  }

  // Custom graphs
  CinderGraph<ListVertex, ListEdge>
  CreateCustomWeightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<ListVertex, ListEdge>(directedOpts);
    } else {
      return CinderGraph<ListVertex, ListEdge>(undirectedOpts);
    }
  }

  CinderGraph<ListVertex, Unweighted>
  CreateCustomUnweightedGraph(GraphOpts structure = GraphOpts::directed) {
    if (structure == GraphOpts::directed) {
      return CinderGraph<ListVertex, Unweighted>(directedOpts);
    } else {
      return CinderGraph<ListVertex, Unweighted>(undirectedOpts);
    }
  }
};

} // namespace CinderPeak
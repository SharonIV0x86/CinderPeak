#pragma once

#include "GraphRuntime.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;
using namespace PeakStore;

class AdjacencyStorageShardTest : public ::testing::Test, public CinderVertex {
protected:
  GraphRuntime runtime;
  AdjacencyList<int, int> intGraph{runtime};
  AdjacencyList<std::string, float> stringGraph{runtime};

  void SetUp() override {
    (void)intGraph.impl_addVertex(1);
    (void)intGraph.impl_addVertex(2);
    (void)intGraph.impl_addVertex(3);
    (void)intGraph.impl_addVertex(4);
    (void)intGraph.impl_addVertex(5);

    (void)intGraph.impl_addVertex(101);
    (void)intGraph.impl_addVertex(102);
    (void)intGraph.impl_addVertex(103);

    (void)stringGraph.impl_addVertex("A");
    (void)stringGraph.impl_addVertex("B");
    (void)stringGraph.impl_addVertex("C");
  }
};

class ComplexAdjVertex : public CinderVertex {
public:
  int vertexData;
  std::string nodeName;
  ComplexAdjVertex(int vertex_data, std::string node_name)
      : vertexData{vertex_data}, nodeName{node_name} {}
  ComplexAdjVertex() = default;
};

class ComplexAdjEdge : public CinderEdge {
public:
  float edgeValue;
  ComplexAdjEdge(float edge_value) : edgeValue{edge_value} {}
  ComplexAdjEdge() = default;
};

class ComplexGraph : public ::testing::Test {
public:
  ComplexAdjVertex v1, v2, v3;
  ComplexAdjEdge e1, e2;
  GraphRuntime runtime;
  AdjacencyList<ComplexAdjVertex, ComplexAdjEdge> complexGraph{runtime};
  ComplexGraph() {
    v1 = ComplexAdjVertex(1, "Vertex1");
    v2 = ComplexAdjVertex(2, "Vertex2");
    v3 = ComplexAdjVertex(3, "Vertex3");

    e1 = ComplexAdjEdge(12.34f);
    e2 = ComplexAdjEdge(76.45f);

    (void)complexGraph.impl_addVertex(v1);
    (void)complexGraph.impl_addVertex(v2);
    (void)complexGraph.impl_addVertex(v3);
  }
};

struct CustomVertex : public CinderVertex {
  int vertex_value;
  std::string name;
  CustomVertex(int v, std::string n) : vertex_value(v), name(n) {}
};
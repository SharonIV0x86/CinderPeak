#include "../common/DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, AddWeightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 15).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2, 25).second);

  EXPECT_EQ(intGraph.numVertices(), 3);
  EXPECT_EQ(intGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddUnweightedEdgePrimitive) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3).second);
  EXPECT_TRUE(intGraph.addEdge(1, 2).second);

  EXPECT_EQ(intGraph.numVertices(), 3);
  EXPECT_EQ(intGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddWeightedEdgeString) {
  auto stringGraph = builder.CreateStringWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "C", 2.5f).second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C", 7.5f).second);
  EXPECT_TRUE(stringGraph.addEdge("A", "B", 12.5f).second);

  EXPECT_EQ(stringGraph.numVertices(), 3);
  EXPECT_EQ(stringGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddUnWeightedEdgeString) {
  auto stringGraph = builder.CreateStringUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(stringGraph.addVertex("A").second);
  EXPECT_TRUE(stringGraph.addVertex("B").second);
  EXPECT_TRUE(stringGraph.addVertex("C").second);

  EXPECT_TRUE(stringGraph.addEdge("A", "C").second);
  EXPECT_TRUE(stringGraph.addEdge("B", "C").second);
  EXPECT_TRUE(stringGraph.addEdge("A", "B").second);

  EXPECT_EQ(stringGraph.numVertices(), 3);
  EXPECT_EQ(stringGraph.numEdges(), 3);
}

TEST_F(CinderGraphFunctionalTest, AddCustomVertexAndEdge) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5), e2(7.0);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(customGraph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(customGraph.numVertices(), 3);
  EXPECT_EQ(customGraph.numEdges(), 2);
}

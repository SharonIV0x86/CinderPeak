#include "../common/DummyGraphBuilder.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(CinderGraphFunctionalTest, ClearWeightedEdges) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3, 15).second);

  EXPECT_EQ(intGraph.numVertices(), 3);
  EXPECT_EQ(intGraph.numEdges(), 2);

  intGraph.clearEdges();
  EXPECT_EQ(intGraph.numEdges(), 0);
  EXPECT_EQ(intGraph.numVertices(), 3); // vertices remain after clear
}

TEST_F(CinderGraphFunctionalTest, ClearUnweightedEdges) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);
  EXPECT_TRUE(intGraph.addVertex(3).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2).second);
  EXPECT_TRUE(intGraph.addEdge(2, 3).second);

  EXPECT_EQ(intGraph.numVertices(), 3);
  EXPECT_EQ(intGraph.numEdges(), 2);

  intGraph.clearEdges();
  EXPECT_EQ(intGraph.numEdges(), 0);
  EXPECT_EQ(intGraph.numVertices(), 3); // vertices remain after clear
}

TEST_F(CinderGraphFunctionalTest, ClearEdgesCustomTypes) {
  auto customGraph = builder.CreateCustomWeightedGraph(GraphOpts::directed);

  ListVertex v1(10);
  ListVertex v2(20);
  ListVertex v3(30);

  EXPECT_TRUE(customGraph.addVertex(v1).second);
  EXPECT_TRUE(customGraph.addVertex(v2).second);
  EXPECT_TRUE(customGraph.addVertex(v3).second);

  ListEdge e1(1.5f);
  ListEdge e2(2.5f);

  EXPECT_TRUE(customGraph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(customGraph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(customGraph.numVertices(), 3);
  EXPECT_EQ(customGraph.numEdges(), 2);

  customGraph.clearEdges();

  EXPECT_EQ(customGraph.numEdges(), 0);
  EXPECT_EQ(customGraph.numVertices(), 3); // vertices remain after clear
}

#include "DummyGraphBuilder.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace CinderPeak;

class BulkInsertionStressTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(BulkInsertionStressTest, BulkVertexInsertion) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  const int numVertices = 1000;

  for (int i = 0; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addVertex(i).second);
  }

  EXPECT_EQ(intGraph.numVertices(), numVertices);

  EXPECT_TRUE(intGraph.hasVertex(0));
  EXPECT_TRUE(intGraph.hasVertex(500));
  EXPECT_TRUE(intGraph.hasVertex(999));
}

TEST_F(BulkInsertionStressTest, BulkWeightedEdgesInsertion) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  const int numVertices = 500;

  for (int i = 0; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addVertex(i).second);
  }

  for (int i = 0; i < numVertices - 1; i++) {
    int src = i;
    int dest = i + 1;
    int weight = i * 5;

    EXPECT_TRUE(intGraph.addEdge(src, dest, weight).second);
  }

  EXPECT_EQ(intGraph.numEdges(), numVertices - 1);

  auto [weight1, status1] = intGraph.getEdge(0, 1);
  EXPECT_TRUE(status1 && weight1 == 0);

  auto [weight2, status2] = intGraph.getEdge(250, 251);
  EXPECT_TRUE(status2 && weight2 == 1250);
}

TEST_F(BulkInsertionStressTest, BulkUnweightedEdgesInsertion) {
  auto intGraph = builder.CreatePrimitiveUnweightedGraph(GraphOpts::directed);

  const int numVertices = 300;

  for (int i = 0; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addVertex(i).second);
  }

  for (int i = 1; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addEdge(0, i).second);
  }

  EXPECT_EQ(intGraph.numEdges(), numVertices - 1);

  auto [weight1, status1] = intGraph.getEdge(0, 1);
  auto [weight2, status2] = intGraph.getEdge(0, 150);
  auto [weight3, status3] = intGraph.getEdge(0, 299);

  EXPECT_TRUE(status1);
  EXPECT_TRUE(status2);
  EXPECT_TRUE(status3);
}

TEST_F(BulkInsertionStressTest, DenseGraph) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  const int numVertices = 50;

  for (int i = 0; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addVertex(i).second);
  }

  int edgeCount = 0;
  for (int i = 0; i < numVertices; i++) {
    for (int j = 1; j <= 10 && (i + j) < numVertices; j++) {
      int src = i;
      int dest = i + j;
      int weight = i * 10 + j;

      EXPECT_TRUE(intGraph.addEdge(src, dest, weight).second);
      edgeCount++;
    }
  }

  EXPECT_EQ(intGraph.numEdges(), edgeCount);

  auto [weight1, status1] = intGraph.getEdge(0, 5);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 5);

  auto [weight2, status2] = intGraph.getEdge(20, 25);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 205);
}

TEST_F(BulkInsertionStressTest, MixedBulkOperations) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  const int batches = 10;
  const int items = 50;

  for (int batch = 0; batch < batches; batch++) {
    for (int i = 0; i < items; i++) {
      int vertex = batch * items + i;
      EXPECT_TRUE(intGraph.addVertex(vertex).second);
    }

    for (int i = 0; i < items - 1; i++) {
      int src = batch * items + i;
      int dest = batch * items + i + 1;

      EXPECT_TRUE(intGraph.addEdge(src, dest, batch * 100 + i).second);
    }

    if (batch > 0) {
      int prev = (batch - 1) * items + (items - 1);
      int curr = batch * items;

      EXPECT_TRUE(intGraph.addEdge(prev, curr, batch * 1000).second);
    }
  }

  EXPECT_EQ(intGraph.numVertices(), batches * items);

  int expectedEdges = batches * (items - 1) + (batches - 1);
  EXPECT_EQ(intGraph.numEdges(), expectedEdges);
}

TEST_F(BulkInsertionStressTest, BulkInsertionWithEdgeUpdates) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  const int numVertices = 200;

  for (int i = 0; i < numVertices; i++) {
    EXPECT_TRUE(intGraph.addVertex(i).second);
  }

  for (int i = 0; i < numVertices - 1; i++) {
    int src = i;
    int dest = i + 1;
    EXPECT_TRUE(intGraph.addEdge(src, dest, i).second);
  }

  for (int i = 0; i < numVertices - 1; i += 10) {
    int src = i;
    int dest = i + 1;
    EXPECT_TRUE(intGraph.updateEdge(src, dest, i * 10).second);
  }

  auto [weight1, status1] = intGraph.getEdge(0, 1);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 0);

  auto [weight2, status2] = intGraph.getEdge(10, 11);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 100);

  auto [weight3, status3] = intGraph.getEdge(20, 21);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 200);
}
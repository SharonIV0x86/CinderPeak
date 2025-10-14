#include "../common/DummyGraphBuilder.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class UpdateAndQueryFlowTest : public ::testing::Test {
protected:
  DummyGraph builder;
};

TEST_F(UpdateAndQueryFlowTest, AddUpdateQuery) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 5).second);

  auto [weight1, status1] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 5);

  auto [newWeight, updated] = intGraph.updateEdge(1, 2, 10);
  EXPECT_TRUE(updated);
  EXPECT_EQ(newWeight, 10);

  auto [weight2, status2] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 10);
}

TEST_F(UpdateAndQueryFlowTest, MultipleUpdates) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(10).second);
  EXPECT_TRUE(intGraph.addVertex(20).second);
  EXPECT_TRUE(intGraph.addEdge(10, 20, 1).second);

  int weights[] = {2, 5, 10, 25, 50};

  for (int w : weights) {
    auto [updatedWeight, success] = intGraph.updateEdge(10, 20, w);
    EXPECT_TRUE(success);
    EXPECT_EQ(updatedWeight, w);

    auto [currentWeight, status] = intGraph.getEdge(10, 20);
    EXPECT_TRUE(status);
    EXPECT_EQ(currentWeight, w);
  }

  auto [finalWeight, status] = intGraph.getEdge(10, 20);
  EXPECT_TRUE(status);
  EXPECT_EQ(finalWeight, 50);
}

TEST_F(UpdateAndQueryFlowTest, MixedOperations) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(100).second);
  EXPECT_TRUE(intGraph.addVertex(200).second);

  EXPECT_TRUE(intGraph.addEdge(100, 200, 5).second);
  auto [weight, status] = intGraph.getEdge(100, 200);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 5);

  intGraph.updateEdge(100, 200, 15);
  auto [weight1, status1] = intGraph.getEdge(100, 200);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 15);

  for (int i = 0; i < 5; i++) {
    auto [weight2, status2] = intGraph.getEdge(100, 200);
    EXPECT_TRUE(status2);
    EXPECT_EQ(weight2, 15);
  }

  intGraph.updateEdge(100, 200, 25);
  auto [weight3, status3] = intGraph.getEdge(100, 200);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 25);
}

TEST_F(UpdateAndQueryFlowTest, RemoveAddQuery) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(1).second);
  EXPECT_TRUE(intGraph.addVertex(2).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 10).second);

  auto [weight, status] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 10);

  EXPECT_TRUE(intGraph.removeEdge(1, 2).second);

  EXPECT_FALSE(intGraph.getEdge(1, 2).second);

  EXPECT_TRUE(intGraph.addEdge(1, 2, 20).second);

  auto [weight2, status2] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 20);

  intGraph.updateEdge(1, 2, 30);

  auto [weight3, status3] = intGraph.getEdge(1, 2);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 30);
}

TEST_F(UpdateAndQueryFlowTest, StressTest) {
  auto intGraph = builder.CreatePrimitiveWeightedGraph(GraphOpts::directed);

  EXPECT_TRUE(intGraph.addVertex(100).second);
  EXPECT_TRUE(intGraph.addVertex(200).second);
  EXPECT_TRUE(intGraph.addEdge(100, 200, 0).second);

  for (int i = 1; i <= 1000; i++) {
    EXPECT_TRUE(intGraph.updateEdge(100, 200, i).second);
    if (i % 100 == 0) {
      auto [currentWeight, status] = intGraph.getEdge(100, 200);
      EXPECT_TRUE(status);
      EXPECT_EQ(currentWeight, i);
    }
  }

  auto [finalWeight, status] = intGraph.getEdge(100, 200);
  EXPECT_TRUE(status);
  EXPECT_EQ(finalWeight, 1000);
}
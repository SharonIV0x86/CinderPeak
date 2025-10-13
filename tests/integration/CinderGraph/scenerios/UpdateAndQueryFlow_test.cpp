#include "CinderGraph.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace CinderPeak;

class UpdateAndQueryFlowTest : public ::testing::Test {
protected:
  void SetUp() override { CinderGraph<int, int>::setConsoleLogging(false); }
};

TEST_F(UpdateAndQueryFlowTest, AddUpdateQuery) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 5).second);

  auto [weight1, status1] = graph.getEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 5);

  auto [newWeight, updated] = graph.updateEdge(1, 2, 10);
  EXPECT_TRUE(updated);
  EXPECT_EQ(newWeight, 10);

  auto [weight2, status2] = graph.getEdge(1, 2);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 10);
}

TEST_F(UpdateAndQueryFlowTest, MultipleUpdates) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(10).second);
  EXPECT_TRUE(graph.addVertex(20).second);
  EXPECT_TRUE(graph.addEdge(10, 20, 1).second);

  int weights[] = {2, 5, 10, 25, 50};

  for (int w : weights) {
    auto [updatedWeight, success] = graph.updateEdge(10, 20, w);
    EXPECT_TRUE(success);
    EXPECT_EQ(updatedWeight, w);

    auto [currentWeight, status] = graph.getEdge(10, 20);
    EXPECT_TRUE(status);
    EXPECT_EQ(currentWeight, w);
  }

  auto [finalWeight, status] = graph.getEdge(10, 20);
  EXPECT_TRUE(status);
  EXPECT_EQ(finalWeight, 50);
}

TEST_F(UpdateAndQueryFlowTest, MixedOperations) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(100).second);
  EXPECT_TRUE(graph.addVertex(200).second);

  EXPECT_TRUE(graph.addEdge(100, 200, 5).second);
  auto [weight, status] = graph.getEdge(100, 200);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 5);

  graph.updateEdge(100, 200, 15);
  auto [weight1, status1] = graph.getEdge(100, 200);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 15);

  for (int i = 0; i < 5; i++) {
    auto [weight2, status2] = graph.getEdge(100, 200);
    EXPECT_TRUE(status2);
    EXPECT_EQ(weight2, 15);
  }

  graph.updateEdge(100, 200, 25);
  auto [weight3, status3] = graph.getEdge(100, 200);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 25);
}

TEST_F(UpdateAndQueryFlowTest, RemoveAddQuery) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 10).second);

  auto [weight, status] = graph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 10);

  EXPECT_TRUE(graph.removeEdge(1, 2).second);

  EXPECT_FALSE(graph.getEdge(1, 2).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 20).second);

  auto [weight2, status2] = graph.getEdge(1, 2);
  EXPECT_TRUE(status2);
  EXPECT_EQ(weight2, 20);

  graph.updateEdge(1, 2, 30);

  auto [weight3, status3] = graph.getEdge(1, 2);
  EXPECT_TRUE(status3);
  EXPECT_EQ(weight3, 30);
}

TEST_F(UpdateAndQueryFlowTest, StressTest) {
  CinderGraph<int, int> graph;

  EXPECT_TRUE(graph.addVertex(100).second);
  EXPECT_TRUE(graph.addVertex(200).second);
  EXPECT_TRUE(graph.addEdge(100, 200, 0).second);

  for (int i = 1; i <= 1000; i++) {
    EXPECT_TRUE(graph.updateEdge(100, 200, i).second);
    if (i % 100 == 0) {
      auto [currentWeight, status] = graph.getEdge(100, 200);
      EXPECT_TRUE(status);
      EXPECT_EQ(currentWeight, i);
    }
  }

  auto [finalWeight, status] = graph.getEdge(100, 200);
  EXPECT_TRUE(status);
  EXPECT_EQ(finalWeight, 1000);
}
#include "CinderPeak.hpp"
#include "gtest/gtest.h"

using namespace CinderPeak;

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

class CinderGraphFunctionalTest : public ::testing::Test {
protected:
  GraphCreationOptions directedOpts;
  GraphCreationOptions undirectedOpts;

  CinderGraphFunctionalTest()
      : directedOpts({GraphCreationOptions::Directed}),
        undirectedOpts({GraphCreationOptions::Undirected}) {}
};

TEST_F(CinderGraphFunctionalTest, UpdateEdgePrimitive) {
  CinderGraph<int, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);

  EXPECT_TRUE(graph.addEdge(1, 2, 25).second);

  auto [weight, status] = graph.getEdge(1, 2);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 25);

  auto [new_weight, status1] = graph.updateEdge(1, 2, 50);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight, 50);

  EXPECT_FALSE(graph.updateEdge(2, 3, 100).second); // edge doesn't exist
}

TEST_F(CinderGraphFunctionalTest, UpdateEdgeString) {
  CinderGraph<std::string, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex("A").second);
  EXPECT_TRUE(graph.addVertex("B").second);

  EXPECT_TRUE(graph.addEdge("A", "B", 42).second);

  auto [weight, status] = graph.getEdge("A", "B");
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 42);

  auto [new_weight, status1] = graph.updateEdge("A", "B", 84);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight, 84);

  EXPECT_FALSE(graph.updateEdge("B", "C", 100).second); // edge doesn't exist
}

TEST_F(CinderGraphFunctionalTest, UpdateCustomEdge) {
  CinderPeak::CinderGraph<ListVertex, ListEdge> graph(directedOpts);

  ListVertex v1(1), v2(2);
  ListEdge e1(3.5), e2(7.0f);

  EXPECT_TRUE(graph.addVertex(v1).second);
  EXPECT_TRUE(graph.addVertex(v2).second);

  EXPECT_TRUE(graph.addEdge(v1, v2, e1).second);

  auto [weight, status] = graph.getEdge(v1, v2);
  ASSERT_TRUE(status);
  EXPECT_EQ(weight->edge_weight, 3.5f);

  auto [new_weight, status1] = graph.updateEdge(v1, v2, e2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(new_weight.edge_weight, 7.0f);

  EXPECT_FALSE(graph.updateEdge(v2, v1, e1).second); // edge doesn't exist
}

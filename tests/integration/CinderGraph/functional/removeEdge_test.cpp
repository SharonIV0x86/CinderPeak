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

TEST_F(CinderGraphFunctionalTest, RemoveWeightedEdgePrimitive) {
  CinderGraph<int, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 3, 5).second);
  EXPECT_TRUE(graph.addEdge(2, 3, 15).second);
  EXPECT_TRUE(graph.addEdge(1, 2, 25).second);

  EXPECT_EQ(graph.numEdges(), 3);

  auto [weight, status] = graph.removeEdge(1, 3);
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 5);

  auto [weight1, status1] = graph.removeEdge(1, 2);
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 25);

  EXPECT_EQ(graph.numEdges(), 1);

  auto [weight2, status2] = graph.removeEdge(1, 6);
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, RemoveUnWeightedEdgePrimitive) {
  CinderGraph<int, Unweighted> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex(1).second);
  EXPECT_TRUE(graph.addVertex(2).second);
  EXPECT_TRUE(graph.addVertex(3).second);

  EXPECT_TRUE(graph.addEdge(1, 3).second);
  EXPECT_TRUE(graph.addEdge(2, 3).second);
  EXPECT_TRUE(graph.addEdge(1, 2).second);

  EXPECT_EQ(graph.numEdges(), 3);

  EXPECT_TRUE(graph.removeEdge(1, 3).second);
  EXPECT_TRUE(graph.removeEdge(1, 2).second);

  EXPECT_EQ(graph.numEdges(), 1);

  EXPECT_FALSE(graph.removeEdge(1, 6).second);
}

TEST_F(CinderGraphFunctionalTest, RemoveWeightedEdgeString) {
  CinderGraph<std::string, int> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex("A").second);
  EXPECT_TRUE(graph.addVertex("B").second);
  EXPECT_TRUE(graph.addVertex("C").second);

  EXPECT_TRUE(graph.addEdge("A", "B", 42).second);
  EXPECT_TRUE(graph.addEdge("B", "C", 100).second);

  EXPECT_EQ(graph.numEdges(), 2);

  auto [weight, status] = graph.removeEdge("A", "B");
  EXPECT_TRUE(status);
  EXPECT_EQ(weight, 42);

  auto [weight1, status1] = graph.removeEdge("B", "C");
  EXPECT_TRUE(status1);
  EXPECT_EQ(weight1, 100);

  EXPECT_EQ(graph.numEdges(), 0);

  auto [weight2, status2] = graph.removeEdge("A", "C");
  EXPECT_FALSE(status2);
  EXPECT_FALSE(weight2.has_value());
}

TEST_F(CinderGraphFunctionalTest, RemoveUnweightedEdgeString) {
  CinderGraph<std::string, Unweighted> graph(directedOpts);

  EXPECT_TRUE(graph.addVertex("A").second);
  EXPECT_TRUE(graph.addVertex("B").second);
  EXPECT_TRUE(graph.addVertex("C").second);

  EXPECT_TRUE(graph.addEdge("A", "B").second);
  EXPECT_TRUE(graph.addEdge("B", "C").second);

  EXPECT_EQ(graph.numEdges(), 2);

  EXPECT_TRUE(graph.removeEdge("A", "B").second);
  EXPECT_TRUE(graph.removeEdge("B", "C").second);

  EXPECT_EQ(graph.numEdges(), 0);

  EXPECT_FALSE(graph.removeEdge("A", "C").second);
}

TEST_F(CinderGraphFunctionalTest, RemoveCustomEdge) {
  CinderPeak::CinderGraph<ListVertex, ListEdge> graph(directedOpts);

  ListVertex v1(1), v2(2), v3(3);
  ListEdge e1(3.5), e2(7.0);

  EXPECT_TRUE(graph.addVertex(v1).second);
  EXPECT_TRUE(graph.addVertex(v2).second);
  EXPECT_TRUE(graph.addVertex(v3).second);

  EXPECT_TRUE(graph.addEdge(v1, v2, e1).second);
  EXPECT_TRUE(graph.addEdge(v2, v3, e2).second);

  EXPECT_EQ(graph.numEdges(), 2);

  auto [weight, status] = graph.removeEdge(v1, v2);
  ASSERT_TRUE(status);
  EXPECT_EQ(weight->edge_weight, 3.5f);

  auto [weight2, status2] = graph.removeEdge(v2, v3);
  ASSERT_TRUE(status2);
  EXPECT_EQ(weight2->edge_weight, 7.0f);

  auto [weight3, status3] = graph.removeEdge(v1, v3);
  ASSERT_FALSE(status3);
  ASSERT_FALSE(weight3.has_value());

  EXPECT_EQ(graph.numEdges(), 0);
}

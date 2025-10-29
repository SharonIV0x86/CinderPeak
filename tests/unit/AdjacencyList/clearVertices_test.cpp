#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, ClearVertices) {
  for (int i = 1; i <= 5; ++i) {
    EXPECT_TRUE(intGraph.impl_hasVertex(i));
  }

  EXPECT_TRUE(intGraph.impl_clearVertices().isOK());

  for (int i = 1; i <= 5; ++i) {
    EXPECT_FALSE(intGraph.impl_hasVertex(i));
  }
}

TEST_F(AdjacencyStorageShardTest, ClearVerticesWithEdges) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 20).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(3, 4, 30).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(4, 5, 40).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(5, 1, 50).isOK());

  EXPECT_TRUE(intGraph.impl_doesEdgeExist(1, 2));
  EXPECT_TRUE(intGraph.impl_doesEdgeExist(2, 3));
  EXPECT_TRUE(intGraph.impl_doesEdgeExist(3, 4));

  EXPECT_TRUE(intGraph.impl_clearVertices().isOK());

  for (int i = 1; i <= 5; ++i) {
    EXPECT_FALSE(intGraph.impl_hasVertex(i));
    EXPECT_FALSE(intGraph.impl_getNeighbors(i).second.isOK());
  }

  EXPECT_FALSE(intGraph.impl_doesEdgeExist(1, 2));
  EXPECT_FALSE(intGraph.impl_doesEdgeExist(2, 3));
  EXPECT_FALSE(intGraph.impl_doesEdgeExist(3, 4));
}

TEST_F(AdjacencyStorageShardTest, ClearVerticesEmptyGraph) {
  AdjacencyList<int, int> emptyGraph{policyHandler};

  EXPECT_TRUE(emptyGraph.impl_clearVertices().isOK());

  EXPECT_FALSE(emptyGraph.impl_hasVertex(1));
}

TEST_F(AdjacencyStorageShardTest, ClearVerticesAndRebuild) {
  intGraph.impl_addEdge(1, 2, 10);
  intGraph.impl_addEdge(2, 3, 20);

  EXPECT_TRUE(intGraph.impl_clearVertices().isOK());

  EXPECT_TRUE(intGraph.impl_addVertex(10).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(20).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(30).isOK());

  EXPECT_TRUE(intGraph.impl_addEdge(10, 20, 100).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(20, 30, 200).isOK());

  EXPECT_TRUE(intGraph.impl_hasVertex(10));
  EXPECT_TRUE(intGraph.impl_hasVertex(20));
  EXPECT_TRUE(intGraph.impl_hasVertex(30));

  auto edge1 = intGraph.impl_getEdge(10, 20);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 100);

  auto edge2 = intGraph.impl_getEdge(20, 30);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 200);

  EXPECT_FALSE(intGraph.impl_hasVertex(1));
  EXPECT_FALSE(intGraph.impl_hasVertex(2));
}

TEST_F(AdjacencyStorageShardTest, ClearVerticesStringGraph) {
  stringGraph.impl_addEdge("A", "B", 1.5f);
  stringGraph.impl_addEdge("B", "C", 2.5f);

  EXPECT_TRUE(stringGraph.impl_hasVertex("A"));
  EXPECT_TRUE(stringGraph.impl_hasVertex("B"));
  EXPECT_TRUE(stringGraph.impl_hasVertex("C"));

  EXPECT_TRUE(stringGraph.impl_clearVertices().isOK());

  EXPECT_FALSE(stringGraph.impl_hasVertex("A"));
  EXPECT_FALSE(stringGraph.impl_hasVertex("B"));
  EXPECT_FALSE(stringGraph.impl_hasVertex("C"));
}

TEST_F(AdjacencyStorageShardTest, ClearVerticesLargeGraph) {
  AdjacencyList<int, int> largeGraph{policyHandler};

  for (int i = 0; i < 100000; ++i) {
    EXPECT_TRUE(largeGraph.impl_addVertex(i).isOK());
  }

  for (int i = 0; i < 99999; ++i) {
    EXPECT_TRUE(largeGraph.impl_addEdge(i, i + 1, i * 10).isOK());
  }

  EXPECT_TRUE(largeGraph.impl_clearVertices().isOK());

  for (int i = 0; i < 100000; ++i) {
    EXPECT_FALSE(largeGraph.impl_hasVertex(i));
  }
}

TEST_F(ComplexGraph, ClearVerticesComplexTypes) {
  complexGraph.impl_addEdge(v1, v2, e1);
  complexGraph.impl_addEdge(v2, v3, e2);

  EXPECT_TRUE(complexGraph.impl_hasVertex(v1));
  EXPECT_TRUE(complexGraph.impl_hasVertex(v2));
  EXPECT_TRUE(complexGraph.impl_hasVertex(v3));

  EXPECT_TRUE(complexGraph.impl_clearVertices().isOK());

  EXPECT_FALSE(complexGraph.impl_hasVertex(v1));
  EXPECT_FALSE(complexGraph.impl_hasVertex(v2));
  EXPECT_FALSE(complexGraph.impl_hasVertex(v3));
}
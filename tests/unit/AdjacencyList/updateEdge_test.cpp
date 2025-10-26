#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, UpdateEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(101, 102, 7).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(103, 102, 5).isOK());

  EXPECT_TRUE(intGraph.impl_updateEdge(101, 102, 10).isOK());
  EXPECT_TRUE(intGraph.impl_updateEdge(103, 102, 1).isOK());
  EXPECT_FALSE(intGraph.impl_updateEdge(400, 102, 1).isOK());

  auto edge1 = intGraph.impl_getEdge(101, 102);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 10);

  auto edge2 = intGraph.impl_getEdge(103, 102);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 1);
}
TEST_F(ComplexGraph, UpdateEdgeOnComplexGraph) {
  ComplexAdjEdge newEdgeValue1(4.3);
  ComplexAdjEdge newEdgeValue2(467.32);
  EXPECT_TRUE(complexGraph.impl_addEdge(v1, v2, e1).isOK());
  EXPECT_TRUE(complexGraph.impl_addEdge(v2, v3, e2).isOK());

  complexGraph.impl_updateEdge(v1, v2, newEdgeValue1);
  EXPECT_EQ(complexGraph.impl_getEdge(v1, v2).first, newEdgeValue1);

  complexGraph.impl_updateEdge(v2, v3, newEdgeValue2);
  EXPECT_EQ(complexGraph.impl_getEdge(v2, v3).first, newEdgeValue2);
}
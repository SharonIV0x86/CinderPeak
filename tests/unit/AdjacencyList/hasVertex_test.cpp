#include "AdjacencyListTestBase.hpp"

TEST_F(AdjacencyStorageShardTest, CheckVertexExistence) {
  EXPECT_TRUE(intGraph.impl_hasVertex(2));
  EXPECT_TRUE(intGraph.impl_hasVertex(5));
  EXPECT_FALSE(intGraph.impl_hasVertex(200));
}
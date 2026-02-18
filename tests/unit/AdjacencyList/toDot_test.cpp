#include "AdjacencyListTestBase.hpp"
#include "CinderGraph.hpp"
#include "gtest/gtest.h"

TEST_F(AdjacencyStorageShardTest, ToDotDirectedGraph) {
  intGraph.impl_clearVertices(); // Start fresh to ensure IDs are 1, 2, 3
  EXPECT_TRUE(intGraph.impl_addVertex(1).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(2).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(3).isOK());

  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 100).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 200).isOK());

  std::string dot = intGraph.impl_toDot(true, false);

  EXPECT_NE(dot.find("digraph"), std::string::npos);
  // With cleared graph, IDs start at 1
  EXPECT_NE(dot.find("node_1 [label=\"1\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_1 -> node_2 [label=\"100\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_2 -> node_3 [label=\"200\"]"), std::string::npos);
}

TEST_F(AdjacencyStorageShardTest, ToDotUndirectedGraph) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(1).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(2).isOK());

  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 50).isOK());

  std::string dot = intGraph.impl_toDot(false, false);

  EXPECT_NE(dot.find("graph"), std::string::npos);
  EXPECT_NE(dot.find("--"), std::string::npos);
  EXPECT_NE(dot.find("node_1 -- node_2 [label=\"50\"]"), std::string::npos);
}

TEST_F(AdjacencyStorageShardTest, ToDotIsolatedNodes) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(10).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(20).isOK());

  std::string dot = intGraph.impl_toDot(true, false);

  // 1st vertex added => ID 1, 2nd => ID 2
  EXPECT_NE(dot.find("node_1 [label=\"10\"]"), std::string::npos);
  EXPECT_NE(dot.find("node_2 [label=\"20\"]"), std::string::npos);
}

TEST_F(AdjacencyStorageShardTest, ToDotFileExport) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(100).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(200).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(100, 200, 50).isOK());

  std::string filename = "test_graph_output.dot";

  GraphCreationOptions opts({GraphCreationOptions::Directed});
  // We cannot easily wrap `intGraph`, so we make a new one.
  CinderGraph<int, int> tempGraph(opts);
  tempGraph.addVertex(100);
  tempGraph.addVertex(200);
  tempGraph.addEdge(100, 200, 50);

  tempGraph.toDot(filename);

  std::ifstream inFile(filename);
  ASSERT_TRUE(inFile.good());

  std::stringstream buffer;
  buffer << inFile.rdbuf();
  std::string content = buffer.str();
  inFile.close();

  EXPECT_NE(content.find("digraph"), std::string::npos);
  EXPECT_NE(content.find("node_1 -> node_2 [label=\"50\"]"), std::string::npos);

  std::remove(filename.c_str());
}

TEST_F(AdjacencyStorageShardTest, ToDotParallelEdges) {
  intGraph.impl_clearVertices();
  EXPECT_TRUE(intGraph.impl_addVertex(1).isOK());
  EXPECT_TRUE(intGraph.impl_addVertex(2).isOK());

  // Directly forcing parallel edges into storage for testing visualization
  // Note: Standard addEdge might reject duplicates depending on policy,
  // but we are testing the visualization of 'WHAT IS THERE'.
  // impl_addEdge in AdjacencyList simply appends, so duplicates are possible if
  // checks are skipped
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 100).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 200).isOK());

  // allowParallel = true
  std::string dot = intGraph.impl_toDot(true, true);

  // Should NOT be strict
  EXPECT_EQ(dot.find("strict"), std::string::npos);
  EXPECT_NE(dot.find("digraph"), std::string::npos);

  // Both edges should appear
  EXPECT_NE(dot.find("label=\"100\""), std::string::npos);
  EXPECT_NE(dot.find("label=\"200\""), std::string::npos);
}

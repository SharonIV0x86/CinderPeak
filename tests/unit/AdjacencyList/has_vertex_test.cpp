#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "CinderPeak.hpp"

using namespace CinderPeak;

TEST(AdjacencyListTest, HasVertexTest) {
    // Create a graph with string vertices and int edge weights
    CinderGraph<std::string, int> graph;
    
    // Test with a vertex that doesn't exist
    EXPECT_FALSE(graph.hasVertex("none"));
    
    // Add a vertex and test again
    graph.addVertex("A");
    EXPECT_TRUE(graph.hasVertex("A"));
    
    // Test with a different vertex that wasn't added
    EXPECT_FALSE(graph.hasVertex("B"));
    
    // Add another vertex and test
    graph.addVertex("B");
    EXPECT_TRUE(graph.hasVertex("B"));
    
    // Test with a non-existent vertex
    EXPECT_FALSE(graph.hasVertex("C"));
}

TEST(AdjacencyListTest, HasVertexWithPrimitiveType) {
    // Test with integer vertices
    CinderGraph<int, int> intGraph;
    
    // Test with a vertex that doesn't exist
    EXPECT_FALSE(intGraph.hasVertex(1));
    
    // Add a vertex and test
    intGraph.addVertex(1);
    EXPECT_TRUE(intGraph.hasVertex(1));
    
    // Test with a different vertex
    EXPECT_FALSE(intGraph.hasVertex(2));
}

TEST(AdjacencyListTest, HasVertexAfterRemoval) {
    CinderGraph<std::string, int> graph;
    
    // Add and then remove a vertex
    graph.addVertex("A");
    EXPECT_TRUE(graph.hasVertex("A"));
    
    graph.removeVertex("A");
    EXPECT_FALSE(graph.hasVertex("A"));
    
    // Test with a vertex that was never added
    EXPECT_FALSE(graph.hasVertex("B"));
}

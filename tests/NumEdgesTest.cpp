//
// Created by ruairi on 05/09/2025.
//

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include "GraphList.hpp"
#include "PeakStore.hpp"
#include "StorageEngine/Utils.hpp"

using namespace CinderPeak;

class NumEdgesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Suppress console output (like other tests do)
        original_cout = std::cout.rdbuf();
        original_cerr = std::cerr.rdbuf();
        std::cout.rdbuf(null_stream.rdbuf());
        std::cerr.rdbuf(null_stream.rdbuf());
    }

    void TearDown() override {
        // Restore console output
        std::cout.rdbuf(original_cout);
        std::cerr.rdbuf(original_cerr);
    }

private:
    std::stringstream null_stream;
    std::streambuf* original_cout = nullptr;
    std::streambuf* original_cerr = nullptr;
};

TEST_F(NumEdgesTest, EmptyGraphHasZeroEdges) {
    GraphList<int, int> graph;
    EXPECT_EQ(graph.numEdges(), 0);
}

TEST_F(NumEdgesTest, GraphWithVerticesButNoEdges) {
    GraphList<int, int> graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    EXPECT_EQ(graph.numEdges(), 0);
}

TEST_F(NumEdgesTest, GraphWithSingleEdge) {
    GraphList<int, int> graph;

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 10);

    EXPECT_EQ(graph.numEdges(), 1);
}

TEST_F(NumEdgesTest, GraphWithMultipleEdges) {
    GraphList<int, int> graph;

    // Add vertices
    for (int i = 1; i <= 5; i++) {
        graph.addVertex(i);
    }

    // Add edges one by one and verify count
    graph.addEdge(1, 2, 10);
    EXPECT_EQ(graph.numEdges(), 1);

    graph.addEdge(2, 3, 20);
    EXPECT_EQ(graph.numEdges(), 2);

    graph.addEdge(3, 4, 30);
    EXPECT_EQ(graph.numEdges(), 3);

    graph.addEdge(4, 5, 40);
    EXPECT_EQ(graph.numEdges(), 4);

    graph.addEdge(5, 1, 50);
    EXPECT_EQ(graph.numEdges(), 5);
}

TEST_F(NumEdgesTest, GraphWithSelfLoop) {
    GraphList<int, int> graph;

    graph.addVertex(1);
    graph.addVertex(2);

    graph.addEdge(1, 2, 10);
    EXPECT_EQ(graph.numEdges(), 1);

    graph.addEdge(1, 1, 20); // Self-loop
    EXPECT_EQ(graph.numEdges(), 2);
}

TEST_F(NumEdgesTest, StringVertexGraph) {
    GraphList<std::string, int> graph;

    graph.addVertex("A");
    graph.addVertex("B");
    graph.addVertex("C");

    EXPECT_EQ(graph.numEdges(), 0);

    graph.addEdge("A", "B", 10);
    EXPECT_EQ(graph.numEdges(), 1);

    graph.addEdge("B", "C", 20);
    EXPECT_EQ(graph.numEdges(), 2);
}

TEST_F(NumEdgesTest, LargerGraph) {
    GraphList<int, int> graph;

    // Add 10 vertices
    for (int i = 1; i <= 10; i++) {
        graph.addVertex(i);
    }

    EXPECT_EQ(graph.numEdges(), 0);

    // Add edges to form a chain: 1->2->3->...->10
    for (int i = 1; i < 10; i++) {
        graph.addEdge(i, i + 1, i * 10);
    }

    EXPECT_EQ(graph.numEdges(), 9);

    // Add some additional edges
    graph.addEdge(1, 5, 100);
    graph.addEdge(3, 8, 200);
    graph.addEdge(4, 9, 300);

    EXPECT_EQ(graph.numEdges(), 12);
}
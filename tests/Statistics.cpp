#include "CinderPeak.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class GraphStatisticsTest : public ::testing::Test {
protected:
  void SetUp() override {
    original_cout = std::cout.rdbuf();
    original_cerr = std::cerr.rdbuf();
    std::cout.rdbuf(null_stream.rdbuf());
    std::cerr.rdbuf(null_stream.rdbuf());
  }

  void TearDown() override {
    std::cout.rdbuf(original_cout);
    std::cerr.rdbuf(original_cerr);
  }

  void displayStats(const std::string &title, const std::string &stats) {
    std::cout.rdbuf(original_cout);
    std::cout << "\n"
              << title << "\n"
              << std::string(40, '=') << "\n"
              << stats << std::endl;
    std::cout.rdbuf(null_stream.rdbuf());
  }

  int extractValue(const std::string &stats, const std::string &label) {
    size_t pos = stats.find(label);
    if (pos == std::string::npos)
      return -1;
    pos += label.length();
    size_t end = stats.find('\n', pos);
    if (end == std::string::npos)
      end = stats.length();
    try {
      return std::stoi(stats.substr(pos, end - pos));
    } catch (...) {
      return -1;
    }
  }

  std::stringstream null_stream;
  std::streambuf *original_cout;
  std::streambuf *original_cerr;
};

// Simple large dense graph test - 1000 vertices, lots of edges
TEST_F(GraphStatisticsTest, LargeDenseGraph) {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  GraphMatrix<int, int> graph(opts);

  const int num_vertices = 1000;
  const int target_edges = 50000; // 50k edges - dense but reasonable

  std::cout.rdbuf(original_cout);
  std::cout << "Creating large graph: " << num_vertices << " vertices, "
            << target_edges << " edges" << std::endl;
  std::cout.rdbuf(null_stream.rdbuf());

  // Add all vertices
  for (int i = 1; i <= num_vertices; ++i) {
    graph.addVertex(i);
  }

  std::mt19937 gen(12345);
  std::uniform_int_distribution<> vertex_dist(1, num_vertices);
  std::uniform_int_distribution<> weight_dist(1, 1000);

  // Make it connected first (spanning tree)
  for (int i = 2; i <= num_vertices; ++i) {
    graph.addEdge(i - 1, i, weight_dist(gen));
  }

  // Add self-loops
  for (int i = 1; i <= 50; ++i) {
    graph.addEdge(i, i, weight_dist(gen));
  }

  // Add random edges until we hit target
  int edges_added = num_vertices - 1 + 50; // spanning tree + self-loops
  for (int attempt = 0;
       attempt < target_edges * 3 && edges_added < target_edges; ++attempt) {
    int v1 = vertex_dist(gen);
    int v2 = vertex_dist(gen);
    try {
      graph.addEdge(v1, v2, weight_dist(gen));
      edges_added++;
    } catch (...) {
      // Skip duplicates/failures
    }

    if (attempt % 10000 == 0) {
      std::cout.rdbuf(original_cout);
      std::cout << "Progress: " << edges_added << " edges added..."
                << std::endl;
      std::cout.rdbuf(null_stream.rdbuf());
    }
  }

  // Get statistics
  std::cout.rdbuf(original_cout);
  std::cout << "Getting statistics..." << std::endl;
  std::cout.rdbuf(null_stream.rdbuf());

  std::string stats = graph.getGraphStatistics();
  displayStats("Large Dense Graph Statistics", stats);

  // Basic verifications
  EXPECT_FALSE(stats.empty());
  EXPECT_NE(stats.find("=== Graph Statistics ==="), std::string::npos);

  int vertices = extractValue(stats, "Vertices: ");
  int edges = extractValue(stats, "Edges: ");
  int self_loops = extractValue(stats, "Self-loops: ");
  int parallel_edges = extractValue(stats, "Parallel edges: ");

  EXPECT_EQ(vertices, num_vertices);
  EXPECT_GT(edges, 1000); // Should have many edges
  EXPECT_GE(self_loops, 0);
  EXPECT_GE(parallel_edges, 0);
}

// Medium size tests for comparison
TEST_F(GraphStatisticsTest, MediumGraphs) {
  std::vector<std::pair<int, int>> configs = {
      {100, 500},  // 100 vertices, 500 edges
      {200, 1000}, // 200 vertices, 1000 edges
      {500, 2500}  // 500 vertices, 2500 edges
  };

  for (auto config : configs) {
    GraphCreationOptions opts({GraphCreationOptions::Undirected});
    GraphMatrix<int, int> graph(opts);

    int vertices = config.first;
    int target_edges = config.second;

    // Add vertices
    for (int i = 1; i <= vertices; ++i) {
      graph.addVertex(i);
    }

    std::mt19937 gen(vertices); // Different seed per test
    std::uniform_int_distribution<> vertex_dist(1, vertices);
    std::uniform_int_distribution<> weight_dist(1, 100);

    // Add edges
    for (int i = 0; i < target_edges; ++i) {
      int v1 = vertex_dist(gen);
      int v2 = vertex_dist(gen);
      try {
        graph.addEdge(v1, v2, weight_dist(gen));
      } catch (...) {
        // Continue on failure
      }
    }

    std::string stats = graph.getGraphStatistics();
    std::string title = "Graph " + std::to_string(vertices) + " vertices";
    displayStats(title, stats);

    EXPECT_FALSE(stats.empty());
    EXPECT_EQ(extractValue(stats, "Vertices: "), vertices);
  }
}

// Original test case (kept for regression)
TEST_F(GraphStatisticsTest, OriginalTest) {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  GraphMatrix<int, int> graph(opts);

  for (int i = 1; i <= 8; ++i) {
    graph.addVertex(i);
  }

  graph.addEdge(1, 2, 50);
  graph.addEdge(2, 3, 60);
  graph.addEdge(3, 4, 70);
  graph.addEdge(4, 5, 80);
  graph.addEdge(5, 6, 90);
  graph.addEdge(5, 5, 90); // Self-loop
  graph.addEdge(6, 5, 90); // Parallel edge
  graph.addEdge(6, 7, 100);
  graph.addEdge(7, 8, 110);
  graph.addEdge(8, 1, 120);
  graph.addEdge(1, 5, 150);
  graph.addEdge(6, 2, 850);

  std::string stats = graph.getGraphStatistics();
  displayStats("Original Test Case", stats);

  EXPECT_NE(stats.find("=== Graph Statistics ==="), std::string::npos);
  EXPECT_EQ(extractValue(stats, "Vertices: "), 8);
  EXPECT_GT(extractValue(stats, "Edges: "), 0);
  EXPECT_GE(extractValue(stats, "Self-loops: "), 1);
}

// Edge cases
TEST_F(GraphStatisticsTest, EdgeCases) {
  // Empty graph
  {
    GraphCreationOptions opts({GraphCreationOptions::Undirected});
    GraphMatrix<int, int> empty_graph(opts);

    std::string stats = empty_graph.getGraphStatistics();
    EXPECT_EQ(extractValue(stats, "Vertices: "), 0);
    EXPECT_EQ(extractValue(stats, "Edges: "), 0);
  }

  // Single vertex with self-loop
  {
    GraphCreationOptions opts({GraphCreationOptions::Undirected});
    GraphMatrix<int, int> single_graph(opts);

    single_graph.addVertex(1);
    single_graph.addEdge(1, 1, 100);

    std::string stats = single_graph.getGraphStatistics();
    displayStats("Single Vertex Test", stats);

    EXPECT_EQ(extractValue(stats, "Vertices: "), 1);
    EXPECT_EQ(extractValue(stats, "Edges: "), 1);
    EXPECT_EQ(extractValue(stats, "Self-loops: "), 1);
  }
}

// Test to validate numVertices functionality
TEST_F(GraphStatisticsTest, NumVertices) {
  GraphList<int, int> graph;

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);
  EXPECT_EQ(graph.numVertices(), 3);

  graph.addVertex(3);
  EXPECT_NE(graph.numEdges(), 4);
}

TEST_F(GraphStatisticsTest, NumEdgesEmptyGraph) {
  GraphList<int, int> graph;

  EXPECT_EQ(graph.numEdges(), 0);

  // Add vertices but no edges
  for (int i = 1; i <= 3; ++i) {
    graph.addVertex(i);
  }
  EXPECT_EQ(graph.numEdges(), 0);
}

TEST_F(GraphStatisticsTest, NumEdgesWithEdges) {
  GraphList<int, int> graph;

  // Add vertices
  for (int i = 1; i <= 4; ++i) {
    graph.addVertex(i);
  }

  // Add edges and verify count
  graph.addEdge(1, 2, 10);
  EXPECT_EQ(graph.numEdges(), 1);

  graph.addEdge(2, 3, 20);
  EXPECT_EQ(graph.numEdges(), 2);

  graph.addEdge(3, 4, 30);
  EXPECT_EQ(graph.numEdges(), 3);
}

TEST_F(GraphStatisticsTest, NumEdgesWithSelfLoop) {
  GraphList<int, int> graph;

  graph.addVertex(1);
  graph.addVertex(2);

  graph.addEdge(1, 2, 10);
  graph.addEdge(1, 1, 20); // Self-loop

  EXPECT_EQ(graph.numEdges(), 2);
}
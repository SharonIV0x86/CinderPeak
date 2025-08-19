#include <chrono>
#include <gtest/gtest.h>
#include <iomanip>
#include <set>
#include <sstream>
#include <vector>

#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore;
using namespace std::chrono;

class HybridCSRCOOTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Setup runs before each test
    graph = std::make_unique<HybridCSR_COO<int, int>>();
  }

  void TearDown() override {
    // Cleanup runs after each test
    graph.reset();
  }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
};

// Basic Functionality Tests
TEST_F(HybridCSRCOOTest, VertexAddition) {
  // Test adding vertices
  auto status1 = graph->impl_addVertex(1);
  auto status2 = graph->impl_addVertex(2);
  auto status3 = graph->impl_addVertex(3);

  EXPECT_TRUE(status1.isOK());
  EXPECT_TRUE(status2.isOK());
  EXPECT_TRUE(status3.isOK());
}

TEST_F(HybridCSRCOOTest, EdgeAddition) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addVertex(3);

  // Test adding edges
  auto edge_status1 = graph->impl_addEdge(1, 2, 10);
  auto edge_status2 = graph->impl_addEdge(2, 3, 20);
  auto edge_status3 = graph->impl_addEdge(1, 3, 15);

  EXPECT_TRUE(edge_status1.isOK());
  EXPECT_TRUE(edge_status2.isOK());
  EXPECT_TRUE(edge_status3.isOK());

  // Test adding edge with non-existent vertex
  auto edge_status_fail = graph->impl_addEdge(1, 99, 5);
  EXPECT_FALSE(edge_status_fail.isOK());
}

TEST_F(HybridCSRCOOTest, EdgeRetrieval) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);
  graph->impl_addVertex(3);

  graph->impl_addEdge(1, 2, 10);
  graph->impl_addEdge(2, 3, 20);
  graph->impl_addEdge(1, 3, 15);

  // Test edge retrieval from COO buffer (hybrid read logic)
  auto [weight1, status1] = graph->impl_getEdge(1, 2);
  auto [weight2, status2] = graph->impl_getEdge(2, 3);
  auto [weight3, status3] = graph->impl_getEdge(1, 3);

  EXPECT_TRUE(status1.isOK());
  EXPECT_EQ(weight1, 10);
  EXPECT_TRUE(status2.isOK());
  EXPECT_EQ(weight2, 20);
  EXPECT_TRUE(status3.isOK());
  EXPECT_EQ(weight3, 15);

  // Test non-existent edge
  auto [weight_fail, status_fail] = graph->impl_getEdge(2, 1);
  EXPECT_FALSE(status_fail.isOK());
}

// Performance Tests
class HybridCSRCOOPerformanceTest : public ::testing::Test {
protected:
  void SetUp() override { graph = std::make_unique<HybridCSR_COO<int, int>>(); }

  void TearDown() override { graph.reset(); }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;

  // Generate completely deterministic edges in a pattern
  std::vector<std::tuple<int, int, int>>
  generatePatternedEdges(int num_vertices, int num_edges) {
    std::vector<std::tuple<int, int, int>> edges;

    // Create edges in a predictable pattern
    for (int i = 0;
         i < num_edges && edges.size() < static_cast<size_t>(num_edges); ++i) {
      int src = i % num_vertices;
      int dest = (i + 1) % num_vertices;

      // Add some variety but keep it deterministic
      if (i % 3 == 0) {
        dest = (i + 2) % num_vertices;
      } else if (i % 5 == 0) {
        dest = (i + 3) % num_vertices;
      }

      // Avoid self-loops
      if (src != dest) {
        edges.emplace_back(src, dest, i + 1);
      }
    }

    return edges;
  }

  template <typename Func>
  void measureOperationCount(Func &&func, const std::string &operation_name,
                             int operation_count) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    double time_us = duration_cast<microseconds>(end - start).count();

    std::cout << "\n--- " << operation_name << " ---\n";
    std::cout << "Operations completed: " << operation_count << "\n";
    std::cout << "Time range: ~" << std::fixed << std::setprecision(0)
              << time_us / 1000 << " ms \n";
    std::cout << "Throughput: ~" << std::fixed << std::setprecision(0)
              << (operation_count * 1000000.0) / time_us << " ops/sec\n";
  }
};

TEST_F(HybridCSRCOOPerformanceTest, VertexInsertionPerformance) {
  const int NUM_VERTICES = 10000;

  auto insertion_func = [&]() {
    graph = std::make_unique<HybridCSR_COO<int, int>>();
    for (int i = 0; i < NUM_VERTICES; ++i) {
      graph->impl_addVertex(i);
    }
  };

  measureOperationCount(insertion_func, "Vertex Insertion Performance",
                        NUM_VERTICES);
}

TEST_F(HybridCSRCOOPerformanceTest, EdgeInsertionPerformance) {
  const int NUM_VERTICES = 1000;
  const int NUM_EDGES = 5000;

  auto edges = generatePatternedEdges(NUM_VERTICES, NUM_EDGES);

  auto insertion_func = [&]() {
    graph = std::make_unique<HybridCSR_COO<int, int>>();

    for (int i = 0; i < NUM_VERTICES; ++i) {
      graph->impl_addVertex(i);
    }

    for (const auto &[src, dest, weight] : edges) {
      graph->impl_addEdge(src, dest, weight);
    }
  };

  measureOperationCount(insertion_func, "Edge Insertion Performance",
                        edges.size());
}

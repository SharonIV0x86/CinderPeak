#include "StorageEngine/HybridCSR_COO.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <random>
#include <set>
#include <vector>

using namespace CinderPeak::PeakStore;
using namespace std::chrono;

class HybridCSRCOOPerformanceTest : public ::testing::Test {
protected:
  void SetUp() override { graph = std::make_unique<HybridCSR_COO<int, int>>(); }

  void TearDown() override { graph.reset(); }

  std::vector<std::tuple<int, int, int>>
  generateTestEdges(int num_vertices, int num_edges, int seed = 42) {
    std::vector<std::tuple<int, int, int>> edges;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> vertex_dis(0, num_vertices - 1);

    std::set<std::pair<int, int>> edge_set;
    while (edges.size() < static_cast<size_t>(num_edges)) {
      int src = vertex_dis(gen);
      int dest = vertex_dis(gen);
      if (src != dest && edge_set.find({src, dest}) == edge_set.end()) {
        edge_set.insert({src, dest});
        edges.emplace_back(src, dest, src * 1000 + dest);
      }
    }
    return edges;
  }

  template <typename Func>
  void measureTime(Func &&func, const std::string &operation_name) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    double time_ms =
        static_cast<double>(duration_cast<microseconds>(end - start).count()) /
        1000.0;
    std::cout << operation_name << ": " << std::fixed << std::setprecision(2)
              << time_ms << " ms\n";
  }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
};

// Test vertex insertion performance
TEST_F(HybridCSRCOOPerformanceTest, VertexInsertionPerformance) {
  const int NUM_VERTICES = 50000;
  auto insertion_func = [&]() {
    for (int i = 0; i < NUM_VERTICES; ++i) {
      graph->impl_addVertex(i);
    }
  };
  measureTime(insertion_func, "50K Vertex Insertion");
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, NUM_VERTICES - 1);
  for (int test = 0; test < 100; ++test) {
    int v = dis(gen);
    EXPECT_FALSE(graph->impl_addVertex(v).isOK())
        << "Adding duplicate vertex " << v << " should fail";
  }
}

// Test edge insertion performance
TEST_F(HybridCSRCOOPerformanceTest, EdgeInsertionPerformance) {
  const int NUM_VERTICES = 5000;
  const int NUM_EDGES = 25000;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    graph->impl_addVertex(i);
  }
  auto edges = generateTestEdges(NUM_VERTICES, NUM_EDGES);
  auto insertion_func = [&]() {
    for (const auto &[src, dest, weight] : edges) {
      graph->impl_addEdge(src, dest, weight);
    }
  };
  measureTime(insertion_func, "25K Edge Insertion");
  std::sample(edges.begin(), edges.end(), edges.begin(),
              std::min(static_cast<size_t>(1000), edges.size()),
              std::mt19937{std::random_device{}()});
  for (int i = 0; i < std::min(1000, static_cast<int>(edges.size())); ++i) {
    auto [src, dest, weight] = edges[i];
    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest, weight))
        << "Edge (" << src << "," << dest << ") should exist";
  }
}

// Test edge retrieval performance
TEST_F(HybridCSRCOOPerformanceTest, EdgeRetrievalPerformance) {
  const int NUM_VERTICES = 1000;
  const int NUM_EDGES = 10000;
  const int NUM_QUERIES = 50000;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    graph->impl_addVertex(i);
  }
  auto edges = generateTestEdges(NUM_VERTICES, NUM_EDGES);
  for (const auto &[src, dest, weight] : edges) {
    graph->impl_addEdge(src, dest, weight);
  }
  std::vector<std::pair<int, int>> queries;
  std::mt19937 gen(123);
  std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);
  for (int i = 0; i < NUM_QUERIES * 0.7 && i < static_cast<int>(edges.size());
       ++i) {
    auto [src, dest, weight] = edges[i % edges.size()];
    queries.emplace_back(src, dest);
  }
  for (size_t i = queries.size(); i < static_cast<size_t>(NUM_QUERIES); ++i) {
    queries.emplace_back(vertex_dis(gen), vertex_dis(gen));
  }
  std::shuffle(queries.begin(), queries.end(), gen);
  auto query_func = [&]() {
    int found = 0;
    for (const auto &[src, dest] : queries) {
      if (graph->impl_doesEdgeExist(src, dest)) {
        found++;
      }
    }
    std::cout << "Found " << found << " edges out of " << NUM_QUERIES
              << " queries\n";
  };
  measureTime(query_func, "50K Edge Queries");
}

// Test mixed operations performance
TEST_F(HybridCSRCOOPerformanceTest, MixedOperationsPerformance) {
  const int NUM_VERTICES = 2000;
  const int INITIAL_EDGES = 5000;
  const int OPERATIONS = 10000;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    graph->impl_addVertex(i);
  }
  auto initial_edges = generateTestEdges(NUM_VERTICES, INITIAL_EDGES);
  for (const auto &[src, dest, weight] : initial_edges) {
    graph->impl_addEdge(src, dest, weight);
  }
  auto mixed_func = [&]() {
    std::mt19937 gen(456);
    std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);
    std::uniform_int_distribution<> op_dis(0, 2);
    int adds = 0, queries = 0, checks = 0;
    for (int i = 0; i < OPERATIONS; ++i) {
      int op = op_dis(gen);
      int src = vertex_dis(gen);
      int dest = vertex_dis(gen);
      if (op == 0 && src != dest) {
        graph->impl_addEdge(src, dest, src * 1000 + dest + i);
        adds++;
      } else if (op == 1) {
        auto [weight, status] = graph->impl_getEdge(src, dest);
        queries++;
      } else {
        graph->impl_doesEdgeExist(src, dest);
        checks++;
      }
    }
    std::cout << "Mixed ops - Adds: " << adds << ", Queries: " << queries
              << ", Checks: " << checks << "\n";
  };
  measureTime(mixed_func, "10K Mixed Operations");
}

// Test large graph correctness
TEST_F(HybridCSRCOOPerformanceTest, LargeGraphCorrectness) {
  const int NUM_VERTICES = 1000;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    ASSERT_TRUE(graph->impl_addVertex(i).isOK())
        << "Adding vertex " << i << " should succeed";
  }
  std::set<std::tuple<int, int, int>> expected_edges;
  for (int i = 0; i < NUM_VERTICES; ++i) {
    for (int j = i + 1; j < std::min(i + 10, NUM_VERTICES); ++j) {
      int weight = i * 1000 + j;
      ASSERT_TRUE(graph->impl_addEdge(i, j, weight).isOK())
          << "Adding edge (" << i << "," << j << ") should succeed";
      expected_edges.insert({i, j, weight});
    }
  }
  for (const auto &[src, dest, expected_weight] : expected_edges) {
    auto [actual_weight, status] = graph->impl_getEdge(src, dest);
    EXPECT_TRUE(status.isOK())
        << "Edge (" << src << "," << dest << ") should exist";
    EXPECT_EQ(actual_weight, expected_weight)
        << "Edge (" << src << "," << dest << ") should have weight "
        << expected_weight;
    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest))
        << "Edge (" << src << "," << dest << ") should exist";
    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest, expected_weight))
        << "Edge (" << src << "," << dest << "," << expected_weight
        << ") should exist";
    EXPECT_FALSE(graph->impl_doesEdgeExist(src, dest, expected_weight + 1))
        << "Edge (" << src << "," << dest << "," << expected_weight + 1
        << ") should not exist";
  }
  std::mt19937 gen(789);
  std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);
  for (int test = 0; test < 1000; ++test) {
    int src = vertex_dis(gen);
    int dest = vertex_dis(gen);
    if (expected_edges.find({src, dest, src * 1000 + dest}) ==
            expected_edges.end() &&
        expected_edges.find({dest, src, dest * 1000 + src}) ==
            expected_edges.end()) {
      EXPECT_FALSE(graph->impl_doesEdgeExist(src, dest))
          << "Unexpected edge (" << src << "," << dest << ") found";
    }
  }
}
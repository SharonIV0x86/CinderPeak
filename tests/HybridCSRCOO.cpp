#include <algorithm>
#include <chrono>
#include <gtest/gtest.h>
#include <iomanip>
#include <random>
#include <set>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore;
using namespace std::chrono;

class HybridCSRCOOTest : public ::testing::Test {
protected:
  void SetUp() override {
    graph = std::make_unique<HybridCSR_COO<int, int>>();
    string_graph = std::make_unique<HybridCSR_COO<std::string, double>>();
  }

  void TearDown() override {
    graph.reset();
    string_graph.reset();
  }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
  std::unique_ptr<HybridCSR_COO<std::string, double>> string_graph;
};

// ==================== BASIC FUNCTIONALITY TESTS ====================

TEST_F(HybridCSRCOOTest, EmptyGraphOperations) {
  // Test operations on empty graph
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(status.isOK());

  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2));
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2, 10));
}

TEST_F(HybridCSRCOOTest, SingleVertexOperations) {
  auto status = graph->impl_addVertex(42);
  EXPECT_TRUE(status.isOK());

  // Adding same vertex should fail
  auto duplicate_status = graph->impl_addVertex(42);
  EXPECT_FALSE(duplicate_status.isOK());

  // Self-loop
  auto self_edge = graph->impl_addEdge(42, 42, 100);
  EXPECT_TRUE(self_edge.isOK());

  auto [weight, get_status] = graph->impl_getEdge(42, 42);
  EXPECT_TRUE(get_status.isOK());
  EXPECT_EQ(weight, 100);
}

TEST_F(HybridCSRCOOTest, VertexAdditionSequential) {
  std::vector<int> vertices = {1, 5, 3, 9, 2, 7};

  for (int v : vertices) {
    auto status = graph->impl_addVertex(v);
    EXPECT_TRUE(status.isOK());
  }

  // Test duplicate additions
  for (int v : vertices) {
    auto status = graph->impl_addVertex(v);
    EXPECT_FALSE(status.isOK());
  }
}

TEST_F(HybridCSRCOOTest, EdgeAdditionBasic) {
  // Setup vertices
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    graph->impl_addVertex(v);
  }

  // Add edges with different patterns
  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());
  EXPECT_TRUE(graph->impl_addEdge(1, 3, 15).isOK());
  EXPECT_TRUE(graph->impl_addEdge(4, 5, 25).isOK());
  EXPECT_TRUE(graph->impl_addEdge(1, 5, 35).isOK());

  // Test edge retrieval
  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK());
  EXPECT_EQ(w1, 10);

  auto [w2, s2] = graph->impl_getEdge(4, 5);
  EXPECT_TRUE(s2.isOK());
  EXPECT_EQ(w2, 25);
}

TEST_F(HybridCSRCOOTest, EdgeWeightUpdation) {
  // Setup vertices
  std::vector<int> vertices = {1, 2, 3, 4, 5};
  for (int v : vertices) {
    graph->impl_addVertex(v);
  }

  // Add edges
  EXPECT_TRUE(graph->impl_addEdge(1, 2, 10).isOK());
  EXPECT_TRUE(graph->impl_addEdge(2, 3, 20).isOK());
  EXPECT_TRUE(graph->impl_addEdge(1, 3, 15).isOK());

  // Edge updation
  EXPECT_TRUE(graph->impl_updateEdge(1, 2, 15).isOK());
  EXPECT_TRUE(graph->impl_updateEdge(2, 3, 10).isOK());
  EXPECT_FALSE(graph->impl_updateEdge(547, 3, 15).isOK());

  auto [w1, s1] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s1.isOK());
  EXPECT_EQ(w1, 15);

  auto [w2, s2] = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(s2.isOK());
  EXPECT_EQ(w2, 10);
}

TEST_F(HybridCSRCOOTest, EdgeAdditionWithNonExistentVertices) {
  graph->impl_addVertex(1);

  // Try to add edge with non-existent source
  auto status1 = graph->impl_addEdge(99, 1, 10);
  EXPECT_FALSE(status1.isOK());

  // Try to add edge with non-existent destination
  auto status2 = graph->impl_addEdge(1, 99, 10);
  EXPECT_FALSE(status2.isOK());

  // Try to add edge with both vertices non-existent
  auto status3 = graph->impl_addEdge(88, 99, 10);
  EXPECT_FALSE(status3.isOK());
}

TEST_F(HybridCSRCOOTest, EdgeRetrievalAdvanced) {
  // Setup a more complex graph
  for (int i = 1; i <= 5; ++i) {
    graph->impl_addVertex(i);
  }

  // Add edges in various patterns
  graph->impl_addEdge(1, 2, 12);
  graph->impl_addEdge(1, 3, 13);
  graph->impl_addEdge(1, 4, 14);
  graph->impl_addEdge(2, 3, 23);
  graph->impl_addEdge(3, 4, 34);
  graph->impl_addEdge(4, 5, 45);

  // Test all edges exist
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2));
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 3));
  EXPECT_TRUE(graph->impl_doesEdgeExist(4, 5));

  // Test with weights
  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2, 12));
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2, 99));

  // Test non-existent edges
  EXPECT_FALSE(graph->impl_doesEdgeExist(2, 1)); // Reverse direction
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 5)); // No direct path
  EXPECT_FALSE(graph->impl_doesEdgeExist(5, 1)); // Reverse
}

// ==================== COO BUFFER FUNCTIONALITY TESTS ====================

TEST_F(HybridCSRCOOTest, COOBufferPriority) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);

  // Add edge that will be in CSR
  graph->impl_addEdge(1, 2, 100);

  // Force CSR build by adding many edges (assuming threshold is 1024)
  for (int i = 3; i < 1027; ++i) {
    graph->impl_addVertex(i);
    graph->impl_addEdge(1, i, i);
  }

  // Now add another edge with same src/dest but different weight
  graph->impl_addEdge(1, 2, 999);

  // COO buffer should have priority - should get the newer weight
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK());
  EXPECT_EQ(weight, 999);
}

TEST_F(HybridCSRCOOTest, COOBufferOverwrite) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);

  // Add multiple edges with same endpoints
  graph->impl_addEdge(1, 2, 10);
  graph->impl_addEdge(1, 2, 20);
  graph->impl_addEdge(1, 2, 30);

  // Should get the most recent one (searches from end)
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK());
  EXPECT_EQ(weight, 30);
}

// ==================== STRING VERTEX TYPE TESTS ====================

TEST_F(HybridCSRCOOTest, StringVertexOperations) {
  // Test with string vertices and double weights
  auto status1 = string_graph->impl_addVertex("alice");
  auto status2 = string_graph->impl_addVertex("bob");
  auto status3 = string_graph->impl_addVertex("charlie");

  EXPECT_TRUE(status1.isOK());
  EXPECT_TRUE(status2.isOK());
  EXPECT_TRUE(status3.isOK());

  // Add edges with fractional weights
  EXPECT_TRUE(string_graph->impl_addEdge("alice", "bob", 1.5).isOK());
  EXPECT_TRUE(string_graph->impl_addEdge("bob", "charlie", 2.7).isOK());
  EXPECT_TRUE(string_graph->impl_addEdge("alice", "charlie", 3.14159).isOK());

  // Test retrieval
  auto [w1, s1] = string_graph->impl_getEdge("alice", "bob");
  EXPECT_TRUE(s1.isOK());
  EXPECT_DOUBLE_EQ(w1, 1.5);

  auto [w2, s2] = string_graph->impl_getEdge("alice", "charlie");
  EXPECT_TRUE(s2.isOK());
  EXPECT_DOUBLE_EQ(w2, 3.14159);

  // Test non-existent edge
  auto [w3, s3] = string_graph->impl_getEdge("charlie", "alice");
  EXPECT_FALSE(s3.isOK());
}

// ==================== EDGE CASES AND ERROR CONDITIONS ====================

TEST_F(HybridCSRCOOTest, NegativeWeights) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);

  // Test negative weights
  EXPECT_TRUE(graph->impl_addEdge(1, 2, -100).isOK());

  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK());
  EXPECT_EQ(weight, -100);

  EXPECT_TRUE(graph->impl_doesEdgeExist(1, 2, -100));
  EXPECT_FALSE(graph->impl_doesEdgeExist(1, 2, 100));
}

TEST_F(HybridCSRCOOTest, ZeroWeights) {
  graph->impl_addVertex(1);
  graph->impl_addVertex(2);

  EXPECT_TRUE(graph->impl_addEdge(1, 2, 0).isOK());

  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(status.isOK());
  EXPECT_EQ(weight, 0);
}

TEST_F(HybridCSRCOOTest, LargeVertexIDs) {
  // Test with large vertex IDs
  int large_id1 = 1000000;
  int large_id2 = 2000000;

  graph->impl_addVertex(large_id1);
  graph->impl_addVertex(large_id2);

  EXPECT_TRUE(graph->impl_addEdge(large_id1, large_id2, 42).isOK());

  auto [weight, status] = graph->impl_getEdge(large_id1, large_id2);
  EXPECT_TRUE(status.isOK());
  EXPECT_EQ(weight, 42);
}

// ==================== ADJACENCY LIST POPULATION TESTS ====================

TEST_F(HybridCSRCOOTest, PopulateFromAdjacencyList) {
  std::unordered_map<int, std::vector<std::pair<int, int>>,
                     CinderPeak::VertexHasher<int>>
      adj_list;

  // Create adjacency list
  adj_list[1] = {{2, 12}, {3, 13}, {4, 14}};
  adj_list[2] = {{3, 23}, {4, 24}};
  adj_list[3] = {{4, 34}};
  adj_list[4] = {};
  adj_list[5] = {{1, 51}, {2, 52}};

  graph->populateFromAdjList(adj_list);

  // Test all edges
  auto [w12, s12] = graph->impl_getEdge(1, 2);
  EXPECT_TRUE(s12.isOK());
  EXPECT_EQ(w12, 12);

  auto [w23, s23] = graph->impl_getEdge(2, 3);
  EXPECT_TRUE(s23.isOK());
  EXPECT_EQ(w23, 23);

  auto [w51, s51] = graph->impl_getEdge(5, 1);
  EXPECT_TRUE(s51.isOK());
  EXPECT_EQ(w51, 51);

  // Test non-existent edge
  auto [w21, s21] = graph->impl_getEdge(2, 1);
  EXPECT_FALSE(s21.isOK());
}

TEST_F(HybridCSRCOOTest, PopulateFromEmptyAdjacencyList) {
  std::unordered_map<int, std::vector<std::pair<int, int>>,
                     CinderPeak::VertexHasher<int>>
      empty_adj_list;

  graph->populateFromAdjList(empty_adj_list);

  // Should handle empty graph gracefully
  auto [weight, status] = graph->impl_getEdge(1, 2);
  EXPECT_FALSE(status.isOK());
}

// ==================== STRESS AND BOUNDARY TESTS ====================

TEST_F(HybridCSRCOOTest, ManyVerticesNoEdges) {
  const int NUM_VERTICES = 10000;

  for (int i = 0; i < NUM_VERTICES; ++i) {
    auto status = graph->impl_addVertex(i);
    EXPECT_TRUE(status.isOK());
  }

  // Test that we can still query
  auto [weight, status] = graph->impl_getEdge(0, 1);
  EXPECT_FALSE(status.isOK());
}

TEST_F(HybridCSRCOOTest, HighDegreeVertex) {
  const int HUB_VERTEX = 0;
  const int NUM_NEIGHBORS = 1000;

  // Create hub vertex
  graph->impl_addVertex(HUB_VERTEX);

  // Add many neighbors
  for (int i = 1; i <= NUM_NEIGHBORS; ++i) {
    graph->impl_addVertex(i);
    EXPECT_TRUE(graph->impl_addEdge(HUB_VERTEX, i, i * 10).isOK());
  }

  // Test random edge retrievals
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, NUM_NEIGHBORS);

  for (int test = 0; test < 100; ++test) {
    int target = dis(gen);
    auto [weight, status] = graph->impl_getEdge(HUB_VERTEX, target);
    EXPECT_TRUE(status.isOK());
    EXPECT_EQ(weight, target * 10);
  }
}

TEST_F(HybridCSRCOOTest, DenseGraph) {
  const int NUM_VERTICES = 100;

  // Add all vertices
  for (int i = 0; i < NUM_VERTICES; ++i) {
    graph->impl_addVertex(i);
  }

  // Add edges between all pairs (complete graph)
  for (int i = 0; i < NUM_VERTICES; ++i) {
    for (int j = 0; j < NUM_VERTICES; ++j) {
      if (i != j) {
        int weight = i * 1000 + j;
        EXPECT_TRUE(graph->impl_addEdge(i, j, weight).isOK());
      }
    }
  }

  // Random testing of edges
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, NUM_VERTICES - 1);

  for (int test = 0; test < 1000; ++test) {
    int src = dis(gen);
    int dest = dis(gen);

    if (src != dest) {
      int expected_weight = src * 1000 + dest;
      auto [weight, status] = graph->impl_getEdge(src, dest);
      EXPECT_TRUE(status.isOK());
      EXPECT_EQ(weight, expected_weight);
    }
  }
}

// ==================== PERFORMANCE TESTS ====================

class HybridCSRCOOPerformanceTest : public ::testing::Test {
protected:
  void SetUp() override { graph = std::make_unique<HybridCSR_COO<int, int>>(); }

  void TearDown() override { graph.reset(); }

  std::unique_ptr<HybridCSR_COO<int, int>> graph;

  // Generate deterministic edges for consistent testing
  std::vector<std::tuple<int, int, int>>
  generateTestEdges(int num_vertices, int num_edges, int seed = 42) {
    std::vector<std::tuple<int, int, int>> edges;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> vertex_dis(0, num_vertices - 1);

    std::set<std::pair<int, int>> edge_set; // Avoid duplicates

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

    double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    std::cout << operation_name << ": " << std::fixed << std::setprecision(2)
              << time_ms << " ms\n";
  }
};

TEST_F(HybridCSRCOOPerformanceTest, VertexInsertionPerformance) {
  const int NUM_VERTICES = 50000;

  auto insertion_func = [&]() {
    for (int i = 0; i < NUM_VERTICES; ++i) {
      graph->impl_addVertex(i);
    }
  };

  measureTime(insertion_func, "50K Vertex Insertion");

  // Verify all vertices were added
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, NUM_VERTICES - 1);

  for (int test = 0; test < 100; ++test) {
    int v = dis(gen);
    // If we try to add again, should fail (already exists)
    EXPECT_FALSE(graph->impl_addVertex(v).isOK());
  }
}

TEST_F(HybridCSRCOOPerformanceTest, EdgeInsertionPerformance) {
  const int NUM_VERTICES = 5000;
  const int NUM_EDGES = 25000;

  // Setup vertices
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

  // Verify random sample of edges
  std::sample(edges.begin(), edges.end(), edges.begin(),
              std::min(static_cast<size_t>(1000), edges.size()),
              std::mt19937{std::random_device{}()});

  for (int i = 0; i < std::min(1000, static_cast<int>(edges.size())); ++i) {
    auto [src, dest, weight] = edges[i];
    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest, weight));
  }
}

TEST_F(HybridCSRCOOPerformanceTest, EdgeRetrievalPerformance) {
  const int NUM_VERTICES = 1000;
  const int NUM_EDGES = 10000;
  const int NUM_QUERIES = 50000;

  // Setup graph
  for (int i = 0; i < NUM_VERTICES; ++i) {
    graph->impl_addVertex(i);
  }

  auto edges = generateTestEdges(NUM_VERTICES, NUM_EDGES);
  for (const auto &[src, dest, weight] : edges) {
    graph->impl_addEdge(src, dest, weight);
  }

  // Generate random queries (mix of existing and non-existing edges)
  std::vector<std::pair<int, int>> queries;
  std::mt19937 gen(123);
  std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);

  // 70% existing edges, 30% random queries
  for (int i = 0; i < NUM_QUERIES * 0.7 && i < static_cast<int>(edges.size());
       ++i) {
    auto [src, dest, weight] = edges[i % edges.size()];
    queries.emplace_back(src, dest);
  }

  for (int i = queries.size(); i < NUM_QUERIES; ++i) {
    queries.emplace_back(vertex_dis(gen), vertex_dis(gen));
  }

  // Shuffle queries
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

TEST_F(HybridCSRCOOPerformanceTest, MixedOperationsPerformance) {
  const int NUM_VERTICES = 2000;
  const int INITIAL_EDGES = 5000;
  const int OPERATIONS = 10000;

  // Initial setup
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
    std::uniform_int_distribution<> op_dis(
        0, 2); // 0=add_edge, 1=query, 2=exist_check

    int adds = 0, queries = 0, checks = 0;

    for (int i = 0; i < OPERATIONS; ++i) {
      int op = op_dis(gen);
      int src = vertex_dis(gen);
      int dest = vertex_dis(gen);

      if (op == 0 && src != dest) { // Add edge
        graph->impl_addEdge(src, dest, src * 1000 + dest + i);
        adds++;
      } else if (op == 1) { // Query weight
        auto [weight, status] = graph->impl_getEdge(src, dest);
        queries++;
      } else { // Check existence
        graph->impl_doesEdgeExist(src, dest);
        checks++;
      }
    }

    std::cout << "Mixed ops - Adds: " << adds << ", Queries: " << queries
              << ", Checks: " << checks << "\n";
  };

  measureTime(mixed_func, "10K Mixed Operations");
}

// ==================== CORRECTNESS VERIFICATION TESTS ====================

TEST_F(HybridCSRCOOPerformanceTest, LargeGraphCorrectness) {
  const int NUM_VERTICES = 1000;

  // Add vertices
  for (int i = 0; i < NUM_VERTICES; ++i) {
    ASSERT_TRUE(graph->impl_addVertex(i).isOK());
  }

  // Add edges in a predictable pattern and keep track
  std::set<std::tuple<int, int, int>> expected_edges;

  for (int i = 0; i < NUM_VERTICES; ++i) {
    for (int j = i + 1; j < std::min(i + 10, NUM_VERTICES); ++j) {
      int weight = i * 1000 + j;
      ASSERT_TRUE(graph->impl_addEdge(i, j, weight).isOK());
      expected_edges.insert({i, j, weight});
    }
  }

  // Verify all expected edges exist with correct weights
  for (const auto &[src, dest, expected_weight] : expected_edges) {
    auto [actual_weight, status] = graph->impl_getEdge(src, dest);
    EXPECT_TRUE(status.isOK())
        << "Edge (" << src << "," << dest << ") not found";
    EXPECT_EQ(actual_weight, expected_weight)
        << "Wrong weight for edge (" << src << "," << dest << ")";

    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest));
    EXPECT_TRUE(graph->impl_doesEdgeExist(src, dest, expected_weight));
    EXPECT_FALSE(graph->impl_doesEdgeExist(src, dest, expected_weight + 1));
  }

  // Verify non-existent edges
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
          << "Unexpected edge found: (" << src << "," << dest << ")";
    }

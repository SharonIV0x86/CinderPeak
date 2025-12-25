#include "AdjacencyListTestBase.hpp"
#include <algorithm>
#include <atomic>
#include <limits>
#include <random>
#include <thread>
#include <vector>

TEST_F(AdjacencyStorageShardTest, CheckVertexExistence) {
  EXPECT_TRUE(intGraph.impl_hasVertex(2));
  EXPECT_TRUE(intGraph.impl_hasVertex(5));
  EXPECT_FALSE(intGraph.impl_hasVertex(200));
}

TEST_F(AdjacencyStorageShardTest, CheckVertexEmptyGraph) {
  PolicyHandler emptyPolicyHandler;
  AdjacencyList<int, int> emptyGraph{emptyPolicyHandler};

  EXPECT_FALSE(emptyGraph.impl_hasVertex(0));
  EXPECT_FALSE(emptyGraph.impl_hasVertex(1));
  EXPECT_FALSE(emptyGraph.impl_hasVertex(-1));
  EXPECT_FALSE(emptyGraph.impl_hasVertex(999));
}

TEST_F(AdjacencyStorageShardTest, CheckVertexMultipleAdditions) {

  AdjacencyList<int, int> graph{policyHandler};
  std::vector<int> vertices = {1, 5, 10, 15, 20, 25, 30};

  for (int v : vertices) {
    graph.impl_addVertex(v);
  }

  for (int v : vertices) {
    EXPECT_TRUE(graph.impl_hasVertex(v));
  }

  EXPECT_FALSE(graph.impl_hasVertex(0));
  EXPECT_FALSE(graph.impl_hasVertex(3));
  EXPECT_FALSE(graph.impl_hasVertex(100));
}

TEST_F(AdjacencyStorageShardTest, CheckVertexDeleted) {

  AdjacencyList<int, int> graph{policyHandler};
  graph.impl_addVertex(100);
  graph.impl_addVertex(200);
  graph.impl_addVertex(300);

  EXPECT_TRUE(graph.impl_hasVertex(200));

  graph.impl_removeVertex(200);
  EXPECT_FALSE(graph.impl_hasVertex(200));
  EXPECT_TRUE(graph.impl_hasVertex(100));
  EXPECT_TRUE(graph.impl_hasVertex(300));
}

TEST_F(AdjacencyStorageShardTest, CheckVertexMix) {

  AdjacencyList<int, int> graph{policyHandler};

  graph.impl_addVertex(75);
  EXPECT_TRUE(graph.impl_hasVertex(75));

  graph.impl_removeVertex(75);
  EXPECT_FALSE(graph.impl_hasVertex(75));

  graph.impl_addVertex(75);
  EXPECT_TRUE(graph.impl_hasVertex(75));
}

TEST_F(AdjacencyStorageShardTest, CheckDeletedVertices) {

  AdjacencyList<int, int> graph{policyHandler};
  std::vector<int> vertices = {1, 2, 3, 4, 5};

  for (int v : vertices) {
    graph.impl_addVertex(v);
  }

  for (int v : vertices) {
    graph.impl_removeVertex(v);
  }

  for (int v : vertices) {
    EXPECT_FALSE(graph.impl_hasVertex(v));
  }
}

TEST_F(AdjacencyStorageShardTest, CheckStringVertices) {
  EXPECT_TRUE(stringGraph.impl_hasVertex("A"));
  EXPECT_TRUE(stringGraph.impl_hasVertex("B"));
  EXPECT_TRUE(stringGraph.impl_hasVertex("C"));
  EXPECT_FALSE(stringGraph.impl_hasVertex("D"));
  EXPECT_FALSE(stringGraph.impl_hasVertex(""));

  stringGraph.impl_addVertex("apple");
  stringGraph.impl_addVertex("banana");

  EXPECT_TRUE(stringGraph.impl_hasVertex("apple"));
  EXPECT_TRUE(stringGraph.impl_hasVertex("banana"));
  EXPECT_FALSE(stringGraph.impl_hasVertex("cherry"));
}

TEST_F(ComplexGraph, CheckComplexVertices) {
  EXPECT_TRUE(complexGraph.impl_hasVertex(v1));
  EXPECT_TRUE(complexGraph.impl_hasVertex(v2));
  EXPECT_TRUE(complexGraph.impl_hasVertex(v3));

  ComplexAdjVertex v4(4, "Vertex4");
  EXPECT_FALSE(complexGraph.impl_hasVertex(v4));

  complexGraph.impl_addVertex(v4);
  EXPECT_TRUE(complexGraph.impl_hasVertex(v4));
}

TEST_F(AdjacencyStorageShardTest, CheckVerticesLargeGraph) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numVertices = 100000;

  for (int i = 0; i < numVertices; i++) {
    graph.impl_addVertex(i);
  }

  for (int i = 0; i < numVertices; i += 100) {
    EXPECT_TRUE(graph.impl_hasVertex(i));
  }

  EXPECT_TRUE(graph.impl_hasVertex(0));
  EXPECT_TRUE(graph.impl_hasVertex(numVertices - 1));
  EXPECT_FALSE(graph.impl_hasVertex(numVertices));
  EXPECT_FALSE(graph.impl_hasVertex(-1));
}

TEST_F(AdjacencyStorageShardTest, ConcurrentReads) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numVertices = 1000;

  for (int i = 0; i < numVertices; i++) {
    graph.impl_addVertex(i);
  }

  const int numThreads = 8;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back([&graph, &successCount, numVertices]() {
      for (int i = 0; i < numVertices; i++) {
        if (graph.impl_hasVertex(i)) {
          successCount++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount.load(), numThreads * numVertices);
}

TEST_F(AdjacencyStorageShardTest, ConcurrentAddAndRead) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numOperations = 500;
  const int numThreads = 4;
  std::vector<std::thread> threads;
  std::atomic<int> addCount{0};

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&graph, &addCount, numOperations, t]() {
      for (int i = 0; i < numOperations; i++) {
        int vertex = t * numOperations + i;
        graph.impl_addVertex(vertex);
        addCount++;
      }
    });
  }

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&graph, numOperations, t]() {
      for (int i = 0; i < numOperations; i++) {
        int vertex = t * numOperations + i;
        graph.impl_hasVertex(vertex);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(addCount.load(), (numThreads / 2) * numOperations);
}

TEST_F(AdjacencyStorageShardTest, ConcurrentDeleteAndRead) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numVertices = 1000;

  for (int i = 0; i < numVertices; i++) {
    graph.impl_addVertex(i);
  }

  const int numThreads = 4;
  std::vector<std::thread> threads;

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&graph, numVertices, t, numThreads]() {
      for (int i = t; i < numVertices; i += numThreads / 2) {
        graph.impl_removeVertex(i);
      }
    });
  }

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&graph, numVertices]() {
      for (int i = 0; i < numVertices; i++) {
        graph.impl_hasVertex(i);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  for (int i = 0; i < numThreads / 2; i++) {
    for (int j = i; j < numVertices; j += numThreads / 2) {
      EXPECT_FALSE(graph.impl_hasVertex(j));
    }
  }
}

TEST_F(AdjacencyStorageShardTest, ConcurrentMixedOperations) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numOperations = 200;
  const int numThreads = 6;
  std::vector<std::thread> threads;
  std::atomic<int> readCount{0};

  for (int i = 0; i < 100; i++) {
    graph.impl_addVertex(i);
  }

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back([&graph, &readCount, numOperations, t]() {
      std::mt19937 rng(t);
      std::uniform_int_distribution<int> dist(0, 299);

      for (int i = 0; i < numOperations; i++) {
        int vertex = dist(rng);
        int op = i % 3;

        if (op == 0) {
          graph.impl_addVertex(vertex);
        } else if (op == 1) {
          graph.impl_removeVertex(vertex);
        } else {
          if (graph.impl_hasVertex(vertex)) {
            readCount++;
          }
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_GT(readCount.load(), 0);
}

TEST_F(AdjacencyStorageShardTest, StressTestConcurrent) {

  AdjacencyList<int, int> graph{policyHandler};
  const int numThreads = 8;
  const int numOperationsPerThread = 1000;
  std::vector<std::thread> threads;
  std::atomic<bool> allTestsPassed{true};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back(
        [&graph, &allTestsPassed, t, numOperationsPerThread]() {
          int baseVertex = t * numOperationsPerThread;

          for (int i = 0; i < numOperationsPerThread; i++) {
            int vertex = baseVertex + i;

            graph.impl_addVertex(vertex);

            if (!graph.impl_hasVertex(vertex)) {
              allTestsPassed = false;
            }

            graph.impl_removeVertex(vertex);

            if (graph.impl_hasVertex(vertex)) {
              allTestsPassed = false;
            }

            graph.impl_addVertex(vertex);

            if (!graph.impl_hasVertex(vertex)) {
              allTestsPassed = false;
            }
          }
        });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_TRUE(allTestsPassed.load());

  for (int t = 0; t < numThreads; t++) {
    int baseVertex = t * numOperationsPerThread;
    for (int i = 0; i < numOperationsPerThread; i++) {
      EXPECT_TRUE(graph.impl_hasVertex(baseVertex + i));
    }
  }
}

TEST_F(AdjacencyStorageShardTest, RaceConditionDetection) {

  AdjacencyList<int, int> graph{policyHandler};
  const int targetVertex = 999;
  const int numThreads = 20;
  std::vector<std::thread> threads;
  std::atomic<int> trueCount{0};
  std::atomic<int> falseCount{0};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back([&graph, &trueCount, &falseCount, targetVertex, t]() {
      for (int i = 0; i < 100; i++) {
        if (t % 2 == 0) {
          graph.impl_addVertex(targetVertex);
        } else {
          graph.impl_removeVertex(targetVertex);
        }

        if (graph.impl_hasVertex(targetVertex)) {
          trueCount++;
        } else {
          falseCount++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_GT(trueCount.load(), 0);
  EXPECT_GT(falseCount.load(), 0);
}
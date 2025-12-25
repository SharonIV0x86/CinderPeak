#include "StorageEngine/HybridCSR_COO.hpp"
#include <algorithm>
#include <atomic>
#include <gtest/gtest.h>
#include <limits>
#include <random>
#include <set>
#include <thread>
#include <vector>

using namespace CinderPeak::PeakStore;

class HybridStorageShardTest : public ::testing::Test {
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

TEST_F(HybridStorageShardTest, HasVertices) {
  graph->impl_addVertex(40);
  graph->impl_addVertex(49);
  EXPECT_TRUE(graph->impl_hasVertex(40));
  EXPECT_TRUE(graph->impl_hasVertex(49));
  EXPECT_FALSE(graph->impl_hasVertex(404));
}

TEST_F(HybridStorageShardTest, HasVertices_EmptyGraph) {
  auto emptyGraph = std::make_unique<HybridCSR_COO<int, int>>();

  EXPECT_FALSE(emptyGraph->impl_hasVertex(0));
  EXPECT_FALSE(emptyGraph->impl_hasVertex(1));
  EXPECT_FALSE(emptyGraph->impl_hasVertex(-1));
  EXPECT_FALSE(emptyGraph->impl_hasVertex(999));
}

TEST_F(HybridStorageShardTest, HasVertices_MultipleAdditions) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  std::vector<int> vertices = {1, 5, 10, 15, 20, 25, 30};

  for (int v : vertices) {
    testGraph->impl_addVertex(v);
  }

  for (int v : vertices) {
    EXPECT_TRUE(testGraph->impl_hasVertex(v));
  }

  EXPECT_FALSE(testGraph->impl_hasVertex(0));
  EXPECT_FALSE(testGraph->impl_hasVertex(3));
  EXPECT_FALSE(testGraph->impl_hasVertex(100));
}

TEST_F(HybridStorageShardTest, HasVertices_Deleted) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  testGraph->impl_addVertex(100);
  testGraph->impl_addVertex(200);
  testGraph->impl_addVertex(300);

  EXPECT_TRUE(testGraph->impl_hasVertex(200));

  testGraph->impl_removeVertex(200);
  EXPECT_FALSE(testGraph->impl_hasVertex(200));
  EXPECT_TRUE(testGraph->impl_hasVertex(100));
  EXPECT_TRUE(testGraph->impl_hasVertex(300));
}

TEST_F(HybridStorageShardTest, HasVertices_Mix) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();

  testGraph->impl_addVertex(75);
  EXPECT_TRUE(testGraph->impl_hasVertex(75));

  testGraph->impl_removeVertex(75);
  EXPECT_FALSE(testGraph->impl_hasVertex(75));

  testGraph->impl_addVertex(75);
  EXPECT_TRUE(testGraph->impl_hasVertex(75));
}

TEST_F(HybridStorageShardTest, HasVertices_DeleteAll) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  std::vector<int> vertices = {1, 2, 3, 4, 5};

  for (int v : vertices) {
    testGraph->impl_addVertex(v);
  }

  for (int v : vertices) {
    testGraph->impl_removeVertex(v);
  }

  for (int v : vertices) {
    EXPECT_FALSE(testGraph->impl_hasVertex(v));
  }
}

TEST_F(HybridStorageShardTest, HasVertices_String) {
  string_graph->impl_addVertex("A");
  string_graph->impl_addVertex("B");
  string_graph->impl_addVertex("C");
  EXPECT_TRUE(string_graph->impl_hasVertex("A"));
  EXPECT_TRUE(string_graph->impl_hasVertex("B"));
  EXPECT_TRUE(string_graph->impl_hasVertex("C"));
  EXPECT_FALSE(string_graph->impl_hasVertex("D"));
  EXPECT_FALSE(string_graph->impl_hasVertex(""));

  string_graph->impl_addVertex("apple");
  string_graph->impl_addVertex("banana");

  EXPECT_TRUE(string_graph->impl_hasVertex("apple"));
  EXPECT_TRUE(string_graph->impl_hasVertex("banana"));
  EXPECT_FALSE(string_graph->impl_hasVertex("cherry"));
}

TEST_F(HybridStorageShardTest, HasVertices_LargeGraph) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numVertices = 100000;

  for (int i = 0; i < numVertices; i++) {
    testGraph->impl_addVertex(i);
  }

  for (int i = 0; i < numVertices; i += 100) {
    EXPECT_TRUE(testGraph->impl_hasVertex(i));
  }

  EXPECT_TRUE(testGraph->impl_hasVertex(0));
  EXPECT_TRUE(testGraph->impl_hasVertex(numVertices - 1));
  EXPECT_FALSE(testGraph->impl_hasVertex(numVertices));
  EXPECT_FALSE(testGraph->impl_hasVertex(-1));
}

TEST_F(HybridStorageShardTest, HasVertices_ConcurrentReads) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numVertices = 1000;

  for (int i = 0; i < numVertices; i++) {
    testGraph->impl_addVertex(i);
  }

  const int numThreads = 8;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back([&testGraph, &successCount, numVertices]() {
      for (int i = 0; i < numVertices; i++) {
        if (testGraph->impl_hasVertex(i)) {
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

TEST_F(HybridStorageShardTest, HasVertices_ConcurrentAddAndRead) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numOperations = 500;
  const int numThreads = 4;
  std::vector<std::thread> threads;
  std::atomic<int> addCount{0};

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&testGraph, &addCount, numOperations, t]() {
      for (int i = 0; i < numOperations; i++) {
        int vertex = t * numOperations + i;
        testGraph->impl_addVertex(vertex);
        addCount++;
      }
    });
  }

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&testGraph, numOperations, t]() {
      for (int i = 0; i < numOperations; i++) {
        int vertex = t * numOperations + i;
        testGraph->impl_hasVertex(vertex);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(addCount.load(), (numThreads / 2) * numOperations);
}

TEST_F(HybridStorageShardTest, HasVertices_ConcurrentDeleteAndRead) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numVertices = 1000;

  for (int i = 0; i < numVertices; i++) {
    testGraph->impl_addVertex(i);
  }

  const int numThreads = 4;
  std::vector<std::thread> threads;

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&testGraph, numVertices, t, numThreads]() {
      for (int i = t; i < numVertices; i += numThreads / 2) {
        testGraph->impl_removeVertex(i);
      }
    });
  }

  for (int t = 0; t < numThreads / 2; t++) {
    threads.emplace_back([&testGraph, numVertices]() {
      for (int i = 0; i < numVertices; i++) {
        testGraph->impl_hasVertex(i);
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  for (int i = 0; i < numThreads / 2; i++) {
    for (int j = i; j < numVertices; j += numThreads / 2) {
      EXPECT_FALSE(testGraph->impl_hasVertex(j));
    }
  }
}

TEST_F(HybridStorageShardTest, HasVertices_ConcurrentMix) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numOperations = 200;
  const int numThreads = 6;
  std::vector<std::thread> threads;
  std::atomic<int> readCount{0};

  for (int i = 0; i < 100; i++) {
    testGraph->impl_addVertex(i);
  }

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back([&testGraph, &readCount, numOperations, t]() {
      std::mt19937 rng(t);
      std::uniform_int_distribution<int> dist(0, 299);

      for (int i = 0; i < numOperations; i++) {
        int vertex = dist(rng);
        int op = i % 3;

        if (op == 0) {
          testGraph->impl_addVertex(vertex);
        } else if (op == 1) {
          testGraph->impl_removeVertex(vertex);
        } else {
          if (testGraph->impl_hasVertex(vertex)) {
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

TEST_F(HybridStorageShardTest, HasVertices_StressTestConcurrent) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int numThreads = 8;
  const int numOperationsPerThread = 1000;
  std::vector<std::thread> threads;
  std::atomic<bool> allTestsPassed{true};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back(
        [&testGraph, &allTestsPassed, t, numOperationsPerThread]() {
          int baseVertex = t * numOperationsPerThread;

          for (int i = 0; i < numOperationsPerThread; i++) {
            int vertex = baseVertex + i;

            testGraph->impl_addVertex(vertex);

            if (!testGraph->impl_hasVertex(vertex)) {
              allTestsPassed = false;
            }

            testGraph->impl_removeVertex(vertex);

            if (testGraph->impl_hasVertex(vertex)) {
              allTestsPassed = false;
            }

            testGraph->impl_addVertex(vertex);

            if (!testGraph->impl_hasVertex(vertex)) {
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
      EXPECT_TRUE(testGraph->impl_hasVertex(baseVertex + i));
    }
  }
}

TEST_F(HybridStorageShardTest, HasVertices_RaceConditionDetection) {
  auto testGraph = std::make_unique<HybridCSR_COO<int, int>>();
  const int targetVertex = 999;
  const int numThreads = 20;
  std::vector<std::thread> threads;
  std::atomic<int> trueCount{0};
  std::atomic<int> falseCount{0};

  for (int t = 0; t < numThreads; t++) {
    threads.emplace_back(
        [&testGraph, &trueCount, &falseCount, targetVertex, t]() {
          for (int i = 0; i < 100; i++) {
            if (t % 2 == 0) {
              testGraph->impl_addVertex(targetVertex);
            } else {
              testGraph->impl_removeVertex(targetVertex);
            }

            if (testGraph->impl_hasVertex(targetVertex)) {
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
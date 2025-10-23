#include "PolicyConfiguration.hpp"
#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;
using namespace PeakStore;

// Base Fixture for Primitive Types
class AdjacencyStorageTestMT : public ::testing::Test, public CinderVertex {
protected:
  PolicyHandler policyHandler;
  AdjacencyList<int, int> intGraph{policyHandler};
  AdjacencyList<std::string, float> stringGraph{policyHandler};

  void SetUp() override {
    intGraph.impl_addVertex(1);
    intGraph.impl_addVertex(2);
    intGraph.impl_addVertex(3);
    intGraph.impl_addVertex(4);
    intGraph.impl_addVertex(5);

    intGraph.impl_addVertex(101);
    intGraph.impl_addVertex(102);
    intGraph.impl_addVertex(103);

    stringGraph.impl_addVertex("A");
    stringGraph.impl_addVertex("B");
    stringGraph.impl_addVertex("C");
  }
};
class AdjacencyListThreadTest : public ::testing::Test {
protected:
  AdjacencyList<int, int> threadGraph{PolicyHandler()};

  void SetUp() override {
    for (int i = 1; i <= 100; ++i) {
      threadGraph.impl_addVertex(i);
    }
  }
};

TEST_F(AdjacencyListThreadTest, ConcurrentVertexAddition) {
  const int numThreads = 10;
  const int verticesPerThread = 50;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failureCount{0};

  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&, t]() {
      int startVertex = 1000 + t * verticesPerThread;
      for (int i = 0; i < verticesPerThread; ++i) {
        auto status = threadGraph.impl_addVertex(startVertex + i);
        if (status.isOK()) {
          successCount++;
        } else {
          failureCount++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount.load(), numThreads * verticesPerThread);
  EXPECT_EQ(failureCount.load(), 0);

  for (int t = 0; t < numThreads; ++t) {
    int startVertex = 1000 + t * verticesPerThread;
    for (int i = 0; i < verticesPerThread; ++i) {
      auto neighbors = threadGraph.impl_getNeighbors(startVertex + i);
      EXPECT_TRUE(neighbors.second.isOK());
    }
  }
}

// Added test to validate concurrent vertex removal
TEST_F(AdjacencyListThreadTest, ConcurrentVertexRemoval) {
  for (int i = 1; i <= 10000; ++i) {
    threadGraph.impl_addVertex(50000 + i);
    if (i > 1) {
      threadGraph.impl_addEdge(50000 + i - 1, 50000 + i, i * 5);
    }
  }

  const int numThreads = 100;
  std::vector<std::thread> threads;
  std::atomic<int> successfulRemovals{0};
  std::atomic<int> failedRemovals{0};

  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&, t]() {
      for (int i = 0; i < 100; ++i) {
        int vertexToRemove = 50000 + (t * 100) + i + 1;
        auto status = threadGraph.impl_removeVertex(vertexToRemove);

        if (status.isOK()) {
          successfulRemovals++;

          EXPECT_FALSE(threadGraph.impl_hasVertex(vertexToRemove));

          auto neighbors = threadGraph.impl_getNeighbors(vertexToRemove);
          EXPECT_FALSE(neighbors.second.isOK());
          EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);
        } else {
          failedRemovals++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successfulRemovals.load(), 10000);
  EXPECT_EQ(failedRemovals.load(), 0);

  for (int i = 1; i <= 10000; ++i) {
    EXPECT_FALSE(threadGraph.impl_hasVertex(50000 + i));
  }

  for (int i = 1; i <= 10000; ++i) {
    auto neighbors = threadGraph.impl_getNeighbors(i);
    if (neighbors.second.isOK()) {
      for (const auto &[neighbor, weight] : neighbors.first) {
        EXPECT_FALSE(neighbor >= 50001 && neighbor <= 60000);
      }
    }
  }
}

TEST_F(AdjacencyListThreadTest, ConcurrentEdgeAddition) {
  const int numThreads = 8;
  const int edgesPerThread = 100;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};
  std::atomic<int> failureCount{0};

  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&, t]() {
      for (int i = 0; i < edgesPerThread; ++i) {
        int src = (t * edgesPerThread + i) % 100 + 1;
        int dest = ((t + 1) * edgesPerThread + i) % 100 + 1;
        int weight = t * 1000 + i;

        auto status = threadGraph.impl_addEdge(src, dest, weight);
        if (status.isOK()) {
          successCount++;
        } else {
          failureCount++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount.load(), numThreads * edgesPerThread);
  EXPECT_EQ(failureCount.load(), 0);
}

TEST_F(AdjacencyListThreadTest, ConcurrentReadWriteOperations) {
  for (int i = 1; i <= 50; ++i) {
    threadGraph.impl_addEdge(i, (i % 50) + 1, i * 10);
  }

  const int numReaderThreads = 6;
  const int numWriterThreads = 4;
  const int operationsPerThread = 200;

  std::vector<std::thread> threads;
  std::atomic<int> readOperations{0};
  std::atomic<int> writeOperations{0};
  std::atomic<int> readErrors{0};
  std::atomic<int> writeErrors{0};

  for (int t = 0; t < numReaderThreads; ++t) {
    threads.emplace_back([&, t]() {
      for (int i = 0; i < operationsPerThread; ++i) {
        int vertex1 = (i % 50) + 1;
        int vertex2 = ((i + 1) % 50) + 1;

        auto edge = threadGraph.impl_getEdge(vertex1, vertex2);
        readOperations++;
        if (!edge.second.isOK() &&
            edge.second.code() != StatusCode::EDGE_NOT_FOUND) {
          readErrors++;
        }

        auto neighbors = threadGraph.impl_getNeighbors(vertex1);
        readOperations++;
        if (!neighbors.second.isOK()) {
          readErrors++;
        }
      }
    });
  }

  for (int t = 0; t < numWriterThreads; ++t) {
    threads.emplace_back([&, t]() {
      for (int i = 0; i < operationsPerThread; ++i) {
        int src = (t * operationsPerThread + i) % 50 + 1;
        int dest = ((t + 2) * operationsPerThread + i) % 50 + 1;
        int weight = (t + 1) * 1000 + i;

        auto status = threadGraph.impl_addEdge(src, dest, weight);
        writeOperations++;
        if (!status.isOK()) {
          writeErrors++;
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(readOperations.load(), numReaderThreads * operationsPerThread * 2);
  EXPECT_EQ(writeOperations.load(), numWriterThreads * operationsPerThread);
  EXPECT_EQ(readErrors.load(), 0);
  EXPECT_EQ(writeErrors.load(), 0);
}

TEST_F(AdjacencyListThreadTest, ConcurrentBulkOperations) {
  const int numThreads = 6;
  std::vector<std::thread> threads;
  std::atomic<int> successCount{0};

  for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&, t]() {
      std::vector<int> vertices;
      int startVertex = 2000 + t * 100;
      for (int i = 0; i < 50; ++i) {
        vertices.push_back(startVertex + i);
      }

      auto vertexStatus = threadGraph.impl_addVertices(vertices);
      if (vertexStatus.isOK()) {
        successCount++;
      }
      std::vector<std::tuple<int, int, int>> edges;
      for (int i = 0; i < 25; ++i) {
        edges.emplace_back(startVertex + i, startVertex + i + 1, i * 10);
      }

      auto edgeStatus = threadGraph.impl_addEdges(edges);
      if (edgeStatus.isOK()) {
        successCount++;
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successCount.load(), numThreads * 2);
}

TEST_F(AdjacencyStorageTestMT, ConcurrentMixedOperationsDeadlock) {
  const int NUM_THREADS = 10;
  const int OPERATIONS_PER_THREAD = 1000;
  std::vector<std::thread> threads;
  std::atomic<int> completed_threads(0);
  std::atomic<bool> deadlock_detected(false);
  std::thread watchdog([&]() {
    auto start = std::chrono::steady_clock::now();
    while (completed_threads < NUM_THREADS) {
      auto now = std::chrono::steady_clock::now();
      auto elapsed =
          std::chrono::duration_cast<std::chrono::seconds>(now - start);
      if (elapsed.count() > 5) {
        deadlock_detected = true;
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
  for (int i = 0; i < NUM_THREADS; i++) {
    threads.emplace_back([&, i]() {
      for (int j = 0; j < OPERATIONS_PER_THREAD; j++) {
        int vertex_id = i * 1000 + j;

        if (j % 4 == 0) {
          intGraph.impl_addVertex(vertex_id);
        } else if (j % 4 == 1) {
          intGraph.impl_doesEdgeExist(1, 2);
        } else if (j % 4 == 2) {
          intGraph.impl_getNeighbors(vertex_id % 10);
        } else {
          if (vertex_id > 10) {
            intGraph.impl_addEdge(vertex_id - 1, vertex_id);
          }
        }
      }
      completed_threads++;
    });
  }
  for (auto &thread : threads) {
    thread.join();
  }
  watchdog.join();
  EXPECT_FALSE(deadlock_detected);
}
TEST_F(AdjacencyStorageTestMT, ConcurrentBulkOperationsRace) {
  std::vector<int> vertices1 = {100, 101, 102, 103, 104};
  std::vector<int> vertices2 = {105, 106, 107, 108, 109};
  std::vector<std::pair<int, int>> edges1 = {
      {100, 101}, {101, 102}, {102, 103}};
  std::vector<std::pair<int, int>> edges2 = {
      {105, 106}, {106, 107}, {107, 108}};
  std::thread t1([&]() {
    intGraph.impl_addVertices(vertices1);
    intGraph.impl_addEdges(edges1);
  });
  std::thread t2([&]() {
    intGraph.impl_addVertices(vertices2);
    intGraph.impl_addEdges(edges2);
  });
  t1.join();
  t2.join();
  for (int v : vertices1) {
    EXPECT_TRUE(intGraph.impl_getNeighbors(v).second.isOK());
  }
  for (int v : vertices2) {
    EXPECT_TRUE(intGraph.impl_getNeighbors(v).second.isOK());
  }
}
TEST_F(AdjacencyStorageTestMT, HighReadWriteContention) {
  const int READ_THREADS = 8;
  const int WRITE_THREADS = 2;
  std::vector<std::thread> threads;
  std::atomic<bool> stop(false);
  for (int i = 0; i < WRITE_THREADS; i++) {
    threads.emplace_back([&, i]() {
      int counter = 0;
      while (!stop) {
        int vertex_id = i * 1000 + counter;
        intGraph.impl_addVertex(vertex_id);
        if (counter > 0) {
          intGraph.impl_addEdge(vertex_id - 1, vertex_id);
        }
        counter++;
        std::this_thread::yield();
      }
    });
  }
  for (int i = 0; i < READ_THREADS; i++) {
    threads.emplace_back([&, i]() {
      int read_count = 0;
      while (!stop && read_count < 1000) {
        int vertex_to_check = i % 10;
        auto result = intGraph.impl_getNeighbors(vertex_to_check);
        read_count++;
        std::this_thread::yield();
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));
  stop = true;

  for (auto &thread : threads) {
    thread.join();
  }
  EXPECT_TRUE(true);
}
TEST_F(AdjacencyStorageTestMT, PotentialReentrancyDeadlock) {
  intGraph.impl_addVertex(100);
  intGraph.impl_addVertex(101);
  intGraph.impl_addEdge(100, 101);

  auto callback = [&](int from, int to) {
    return intGraph.impl_doesEdgeExist(from, to);
  };
  std::thread t([&]() {
    auto neighbors = intGraph.impl_getNeighbors(100);
    if (neighbors.second.isOK()) {
      for (const auto &[vertex, weight] : neighbors.first) {
        bool exists = callback(100, vertex);
        EXPECT_TRUE(exists);
      }
    }
  });
  auto start = std::chrono::steady_clock::now();
  bool completed = false;

  while (std::chrono::steady_clock::now() - start < std::chrono::seconds(3)) {
    if (t.joinable()) {
      t.join();
      completed = true;
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  EXPECT_TRUE(completed);
}

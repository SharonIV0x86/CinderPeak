#include <atomic>
#include <gtest/gtest.h>
#include <thread>

#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore;
using namespace std::chrono;

class HybridStorageShardTestMT : public ::testing::Test {
protected:
  void SetUp() override {
    graph = std::make_unique<HybridCSR_COO<int, int>>();
    string_graph = std::make_unique<HybridCSR_COO<std::string, double>>();
  }

  void TearDown() override {
    graph.reset();
    string_graph.reset();
  }

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

  std::unique_ptr<HybridCSR_COO<int, int>> graph;
  std::unique_ptr<HybridCSR_COO<std::string, double>> string_graph;
};

TEST_F(HybridStorageShardTestMT, ConcurrentReads) {
  for (int i = 0; i < 100; ++i) {
    graph->impl_addVertex(i);
  }
  for (int i = 0; i < 100; ++i) {
    graph->impl_addEdge(i, (i + 1) % 100, i * 10);
  }

  std::atomic<bool> stop_flag{false};
  auto reader = [&]() {
    while (!stop_flag.load()) {
      auto [w, status] = graph->impl_getEdge(10, 11);
      EXPECT_TRUE(status.isOK());
      EXPECT_EQ(w, 100);
    }
  };

  std::thread t1(reader), t2(reader), t3(reader);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  stop_flag.store(true);
  t1.join();
  t2.join();
  t3.join();
}

TEST_F(HybridStorageShardTestMT, MixedReadWrite) {
  for (int i = 0; i < 50; ++i)
    graph->impl_addVertex(i);

  std::atomic<bool> stop_flag{false};
  auto writer = [&]() {
    int counter = 0;
    while (!stop_flag.load()) {
      graph->impl_addEdge(counter % 50, (counter + 1) % 50, counter);
      counter++;
    }
  };

  auto reader = [&]() {
    while (!stop_flag.load()) {
      auto [w, status] = graph->impl_getEdge(5, 6);
      (void)status;
    }
  };

  std::thread tw(writer);
  std::thread tr1(reader), tr2(reader);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  stop_flag.store(true);
  tw.join();
  tr1.join();
  tr2.join();
}

TEST_F(HybridStorageShardTestMT, StressTestMultipleThreads) {
  const int NUM_THREADS = 8;
  for (int i = 0; i < 500; ++i)
    graph->impl_addVertex(i);

  auto worker = [&](int id) {
    for (int i = 0; i < 2000; ++i) {
      int src = (i + id) % 500;
      int dst = (i * 13 + id) % 500;
      if (src != dst)
        graph->impl_addEdge(src, dst, src + dst);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(worker, i);
  for (auto &t : threads)
    t.join();

  auto [w, status] = graph->impl_getEdge(1, 2);
  if (status.isOK())
    SUCCEED() << "Edge found after stress test.";
}

TEST_F(HybridStorageShardTestMT, ProperRaceDetection) {
  for (int i = 0; i < 10; ++i)
    graph->impl_addVertex(i);

  std::atomic<bool> stop_flag{false};
  std::atomic<int> write_counter{0};

  auto writer = [&]() {
    while (!stop_flag.load()) {
      int val = write_counter.fetch_add(1);
      graph->impl_addEdge(0, 1, val);
    }
  };

  auto reader = [&]() {
    while (!stop_flag.load()) {
      auto [w, status] = graph->impl_getEdge(0, 1);
      (void)w;
      (void)status;
    }
  };

  std::thread t1(writer);
  std::thread r1(reader), r2(reader), r3(reader);

  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  stop_flag.store(true);

  t1.join();
  r1.join();
  r2.join();
  r3.join();

  auto [final_w, final_status] = graph->impl_getEdge(0, 1);
  EXPECT_TRUE(final_status.isOK());
}

TEST_F(HybridStorageShardTestMT, PerformanceRegression) {
  for (int i = 0; i < 2000; ++i)
    graph->impl_addVertex(i);

  auto edges = generateTestEdges(2000, 5000);
  for (auto &[src, dst, w] : edges)
    graph->impl_addEdge(src, dst, w);

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 10000; ++i) {
    int src = i % 2000;
    int dst = (i * 17) % 2000;
    graph->impl_doesEdgeExist(src, dst);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
          .count();

  EXPECT_LT(duration_ms, 500) << "Performance regression: took too long!";
}

TEST_F(HybridStorageShardTestMT, ConcurrentVertexAdditionAndRemoval) {
  const int NUM_THREADS = 6;
  const int NUM_OPERATIONS = 1000;

  std::atomic<bool> stop_flag{false};
  std::atomic<int> vertex_counter{50};

  for (int i = 0; i < 50; ++i)
    graph->impl_addVertex(i);

  auto adder = [&]() {
    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int v = vertex_counter.fetch_add(1);
      graph->impl_addVertex(v);
    }
  };

  auto remover = [&]() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 149);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int v = dis(gen);
      graph->impl_removeVertex(v);
    }
  };

  auto reader = [&]() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 149);

    while (!stop_flag.load()) {
      int v = dis(gen);
      bool exists = graph->impl_hasVertex(v);

      (void)exists;
    }
  };

  std::vector<std::thread> threads;

  for (int i = 0; i < 2; ++i)
    threads.emplace_back(adder);

  for (int i = 0; i < 2; ++i)
    threads.emplace_back(remover);

  for (int i = 0; i < 2; ++i)
    threads.emplace_back(reader);

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  stop_flag.store(true);

  for (auto &t : threads)
    t.join();

  SUCCEED()
      << "Concurrent vertex addition and removal completed without crashing";
}

TEST_F(HybridStorageShardTestMT, EdgeUpdateRaceConditions) {
  const int NUM_THREADS = 4;
  const int NUM_UPDATES = 1000;

  for (int i = 0; i < 10; ++i)
    graph->impl_addVertex(i);

  graph->impl_addEdge(0, 1, 100);

  std::vector<std::thread> threads;
  std::atomic<int> update_count{0};

  auto updater = [&](int thread_id) {
    for (int i = 0; i < NUM_UPDATES; ++i) {
      int new_weight = thread_id * 1000 + i;
      auto status = graph->impl_updateEdge(0, 1, new_weight);
      if (status.isOK()) {
        update_count.fetch_add(1);
      }
    }
  };

  auto reader = [&]() {
    for (int i = 0; i < NUM_UPDATES * 2; ++i) {
      auto [weight, status] = graph->impl_getEdge(0, 1);
      if (status.isOK()) {

        EXPECT_GE(weight, 0);
      }
    }
  };

  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(updater, i);

  for (int i = 0; i < 2; ++i)
    threads.emplace_back(reader);

  for (auto &t : threads)
    t.join();

  auto [final_weight, final_status] = graph->impl_getEdge(0, 1);
  EXPECT_TRUE(final_status.isOK());
  EXPECT_GE(final_weight, 0);
}

TEST_F(HybridStorageShardTestMT, MixedOperationsStressTest) {
  const int NUM_THREADS = 10;
  const int NUM_OPERATIONS = 2000;
  const int NUM_VERTICES = 100;

  for (int i = 0; i < NUM_VERTICES; ++i)
    graph->impl_addVertex(i);

  std::atomic<bool> stop_flag{false};
  std::atomic<int> operations_count{0};

  auto mixed_worker = [&](int thread_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 4);
    std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);
    std::uniform_int_distribution<> weight_dis(1, 1000);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int op = op_dis(gen);
      int src = vertex_dis(gen);
      int dst = vertex_dis(gen);

      while (src == dst) {
        dst = vertex_dis(gen);
      }

      switch (op) {
      case 0:
        graph->impl_addEdge(src, dst, weight_dis(gen));
        break;
      case 1:
        graph->impl_doesEdgeExist(src, dst);
        break;
      case 2:
        graph->impl_getEdge(src, dst);
        break;
      case 3:
        graph->impl_updateEdge(src, dst, weight_dis(gen));
        break;
      case 4:
        graph->impl_hasVertex(src);
        break;
      }

      operations_count.fetch_add(1);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(mixed_worker, i);

  for (auto &t : threads)
    t.join();

  EXPECT_GE(operations_count.load(), NUM_THREADS * NUM_OPERATIONS);
  SUCCEED() << "Mixed operations stress test completed without crashes";
}

TEST_F(HybridStorageShardTestMT, BuildDuringConcurrentAccess) {
  const int NUM_THREADS = 4;

  for (int i = 0; i < 1000; ++i)
    graph->impl_addVertex(i);

  for (int i = 0; i < 5000; ++i)
    graph->impl_addEdge(i % 1000, (i + 1) % 1000, i);

  std::atomic<bool> stop_flag{false};

  auto reader = [&]() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999);

    while (!stop_flag.load()) {
      int src = dis(gen);
      int dst = dis(gen);
      graph->impl_getEdge(src, dst);
    }
  };

  auto writer = [&]() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> vertex_dis(1000, 1500);
    std::uniform_int_distribution<> weight_dis(1, 100);

    for (int i = 0; i < 100; ++i) {
      int src = vertex_dis(gen);
      int dst = vertex_dis(gen);

      if (src != dst) {
        graph->impl_addVertex(src);
        graph->impl_addVertex(dst);
        graph->impl_addEdge(src, dst, weight_dis(gen));
      }
    }
  };

  std::vector<std::thread> threads;

  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(reader);

  threads.emplace_back(writer);

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  stop_flag.store(true);

  for (auto &t : threads)
    t.join();

  SUCCEED() << "Concurrent access during structure building completed without "
               "crashes";
}

TEST_F(HybridStorageShardTestMT, ClearEdgesUnderLoad) {
  const int NUM_THREADS = 4;

  for (int i = 0; i < 100; ++i)
    graph->impl_addVertex(i);

  for (int i = 0; i < 500; ++i)
    graph->impl_addEdge(i % 100, (i + 1) % 100, i);

  std::atomic<bool> stop_flag{false};

  auto reader = [&]() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 99);

    while (!stop_flag.load()) {
      int src = dis(gen);
      int dst = dis(gen);
      graph->impl_getEdge(src, dst);
    }
  };

  auto clear_edges = [&]() {
    for (int i = 0; i < 5; ++i) {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      graph->impl_clearEdges();
    }
  };

  std::vector<std::thread> threads;

  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(reader);

  threads.emplace_back(clear_edges);

  std::this_thread::sleep_for(std::chrono::milliseconds(300));
  stop_flag.store(true);

  for (auto &t : threads)
    t.join();

  SUCCEED() << "Clear edges under load test completed without crashes";
}

TEST_F(HybridStorageShardTestMT, StringTypeConcurrency) {
  const int NUM_THREADS = 4;
  const int NUM_OPERATIONS = 1000;

  string_graph->impl_addVertex("vertex0");
  string_graph->impl_addVertex("vertex1");
  string_graph->impl_addVertex("vertex2");

  std::atomic<bool> stop_flag{false};
  std::atomic<int> operations_count{0};

  auto worker = [&](int thread_id) {
    std::vector<std::string> vertices = {"vertex0", "vertex1", "vertex2"};
    std::vector<std::string> new_vertices = {"vertex3", "vertex4", "vertex5",
                                             "vertex6", "vertex7"};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 3);
    std::uniform_int_distribution<> vertex_dis(0, 2);
    std::uniform_int_distribution<> new_vertex_dis(0, 4);
    std::uniform_real_distribution<> weight_dis(1.0, 100.0);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int op = op_dis(gen);

      switch (op) {
      case 0: {
        int src_idx = vertex_dis(gen);
        int dst_idx = vertex_dis(gen);
        if (src_idx != dst_idx) {
          string_graph->impl_addEdge(vertices[src_idx], vertices[dst_idx],
                                     weight_dis(gen));
        }
        break;
      }
      case 1: {
        int src_idx = vertex_dis(gen);
        int dst_idx = vertex_dis(gen);
        string_graph->impl_getEdge(vertices[src_idx], vertices[dst_idx]);
        break;
      }
      case 2:
        string_graph->impl_addVertex(new_vertices[new_vertex_dis(gen)]);
        break;
      case 3:
        string_graph->impl_hasVertex(vertices[vertex_dis(gen)]);
        break;
      }

      operations_count.fetch_add(1);
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(worker, i);

  for (auto &t : threads)
    t.join();

  EXPECT_GE(operations_count.load(), NUM_THREADS * NUM_OPERATIONS);
  SUCCEED() << "String type concurrency test completed without crashes";
}

TEST_F(HybridStorageShardTestMT, MemoryConsistencyAfterConcurrentOperations) {
  const int NUM_THREADS = 6;
  const int NUM_OPERATIONS = 1000;
  const int NUM_VERTICES = 20;

  for (int i = 0; i < NUM_VERTICES; ++i)
    graph->impl_addVertex(i);

  auto worker = [&](int thread_id) {
    std::random_device rd;
    std::mt19937 gen(rd() + thread_id);
    std::uniform_int_distribution<> op_dis(0, 1);
    std::uniform_int_distribution<> vertex_dis(0, NUM_VERTICES - 1);
    std::uniform_int_distribution<> weight_dis(1, 100);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int src = vertex_dis(gen);
      int dst = vertex_dis(gen);

      while (src == dst) {
        dst = vertex_dis(gen);
      }

      switch (op_dis(gen)) {
      case 0:
        graph->impl_addEdge(src, dst, weight_dis(gen));
        break;
      case 1:
        graph->impl_updateEdge(src, dst, weight_dis(gen));
        break;
      }
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(worker, i);

  for (auto &t : threads)
    t.join();

  for (int i = 0; i < NUM_VERTICES; ++i) {
    bool exists = graph->impl_hasVertex(i);
    EXPECT_TRUE(exists) << "Vertex " << i
                        << " should exist after concurrent operations";
  }

  SUCCEED() << "Memory consistency test completed without crashes";
}
TEST_F(HybridStorageShardTestMT, HighContentionOnSingleVertex) {
  const int NUM_THREADS = 8;
  const int NUM_OPERATIONS = 2000;

  graph->impl_addVertex(0);
  graph->impl_addVertex(1);
  graph->impl_addEdge(0, 1, 100);

  std::atomic<bool> stop_flag{false};
  std::atomic<int> success_count{0};

  auto high_contention_worker = [&](int thread_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 3);
    std::uniform_int_distribution<> weight_dis(1, 1000);

    for (int i = 0; i < NUM_OPERATIONS; ++i) {
      int op = op_dis(gen);

      switch (op) {
      case 0:
        if (graph->impl_updateEdge(0, 1, weight_dis(gen)).isOK()) {
          success_count.fetch_add(1);
        }
        break;
      case 1:
        graph->impl_getEdge(0, 1);
        break;
      case 2:
        graph->impl_doesEdgeExist(0, 1);
        break;
      case 3:
        graph->impl_addEdge(0, 1, weight_dis(gen));
        break;
      }
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(high_contention_worker, i);

  for (auto &t : threads)
    t.join();

  auto [final_weight, final_status] = graph->impl_getEdge(0, 1);
  EXPECT_TRUE(final_status.isOK());
  EXPECT_GE(final_weight, 0);

  SUCCEED() << "High contention test completed without crashes. Success count: "
            << success_count.load();
}

TEST_F(HybridStorageShardTestMT, LongRunningStressTest) {
  const int NUM_THREADS = 8;
  const int DURATION_MS = 5000;

  for (int i = 0; i < 100; ++i)
    graph->impl_addVertex(i);

  std::atomic<bool> stop_flag{false};
  std::atomic<int> operations_count{0};

  auto long_running_worker = [&](int thread_id) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> op_dis(0, 4);
    std::uniform_int_distribution<> vertex_dis(0, 99);
    std::uniform_int_distribution<> weight_dis(1, 1000);

    auto start = std::chrono::steady_clock::now();

    while (!stop_flag.load()) {
      int op = op_dis(gen);
      int src = vertex_dis(gen);
      int dst = vertex_dis(gen);

      while (src == dst) {
        dst = vertex_dis(gen);
      }

      switch (op) {
      case 0:
        graph->impl_addEdge(src, dst, weight_dis(gen));
        break;
      case 1:
        graph->impl_getEdge(src, dst);
        break;
      case 2:
        graph->impl_updateEdge(src, dst, weight_dis(gen));
        break;
      case 3:
        graph->impl_hasVertex(src);
        break;
      case 4:
        graph->impl_doesEdgeExist(src, dst);
        break;
      }

      operations_count.fetch_add(1);

      auto now = std::chrono::steady_clock::now();
      if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
              .count() >= DURATION_MS) {
        break;
      }
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < NUM_THREADS; ++i)
    threads.emplace_back(long_running_worker, i);
  std::this_thread::sleep_for(std::chrono::milliseconds(DURATION_MS));
  stop_flag.store(true);

  for (auto &t : threads)
    t.join();

  EXPECT_GT(operations_count.load(), 0);
  SUCCEED() << "Long running stress test completed. Operations: "
            << operations_count.load();
}

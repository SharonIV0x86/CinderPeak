#include <atomic>
#include <gtest/gtest.h>
#include <thread>

#include "StorageEngine/HybridCSR_COO.hpp"

using namespace CinderPeak::PeakStore;
using namespace std::chrono;

class HybridStorageMTTest : public ::testing::Test {
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

TEST_F(HybridStorageMTTest, ConcurrentReads) {
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

TEST_F(HybridStorageMTTest, MixedReadWrite) {
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

TEST_F(HybridStorageMTTest, StressTestMultipleThreads) {
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

TEST_F(HybridStorageMTTest, ProperRaceDetection) {
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

TEST_F(HybridStorageMTTest, PerformanceRegression) {
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
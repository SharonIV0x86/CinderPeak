#include "CinderPeak.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <thread>

using namespace CinderPeak::PeakStore;
using namespace CinderPeak;

class GraphStatisticsThreadTest : public ::testing::Test {
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

TEST_F(GraphStatisticsThreadTest, ConcurrentReadOperations) {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  CinderGraph<int, int> graph(opts);

  for (int i = 1; i <= 100; ++i) {
    graph.addVertex(i);
  }

  std::mt19937 gen(42);
  std::uniform_int_distribution<> vertex_dist(1, 100);
  std::uniform_int_distribution<> weight_dist(1, 100);

  for (int i = 0; i < 200; ++i) {
    int v1 = vertex_dist(gen);
    int v2 = vertex_dist(gen);
    try {
      graph.addEdge(v1, v2, weight_dist(gen));
    } catch (...) {
    }
  }

  const int num_threads = 10;
  const int reads_per_thread = 1000;
  std::vector<std::thread> threads;
  std::atomic<int> successful_reads{0};
  std::atomic<bool> test_failed{false};

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(
        [&graph, &successful_reads, &test_failed, reads_per_thread]() {
          try {
            for (int j = 0; j < reads_per_thread; ++j) {
              size_t vertices = graph.numVertices();
              size_t edges = graph.numEdges();
              std::string stats = graph.getGraphStatistics();

              if (vertices != 100 || stats.empty()) {
                test_failed = true;
                return;
              }

              successful_reads++;
            }
          } catch (...) {
            test_failed = true;
          }
        });
  }

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_FALSE(test_failed);
  EXPECT_EQ(successful_reads.load(), num_threads * reads_per_thread);
  EXPECT_EQ(graph.numVertices(), 100);
}

TEST_F(GraphStatisticsThreadTest, ConcurrentWriteOperations) {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  CinderGraph<int, int> graph(opts);

  for (int i = 1; i <= 50; ++i) {
    graph.addVertex(i);
  }

  const int num_threads = 8;
  const int operations_per_thread = 100;
  std::vector<std::thread> threads;
  std::atomic<int> successful_operations{0};
  std::atomic<bool> test_failed{false};

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back([&graph, &successful_operations, &test_failed,
                          operations_per_thread, i]() {
      try {
        std::mt19937 gen(i * 1000);
        std::uniform_int_distribution<> vertex_dist(1, 50);
        std::uniform_int_distribution<> weight_dist(1, 100);

        for (int j = 0; j < operations_per_thread; ++j) {
          int v1 = vertex_dist(gen);
          int v2 = vertex_dist(gen);

          try {
            graph.addEdge(v1, v2, weight_dist(gen));
            successful_operations++;
          } catch (...) {
          }
        }
      } catch (...) {
        test_failed = true;
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_FALSE(test_failed);
  EXPECT_EQ(graph.numVertices(), 50);
  EXPECT_GT(successful_operations.load(), 0);
  std::string stats = graph.getGraphStatistics();
  EXPECT_FALSE(stats.empty());
  EXPECT_EQ(extractValue(stats, "Vertices: "), 50);
}

TEST_F(GraphStatisticsThreadTest, MixedReadWriteOperations) {
  GraphCreationOptions opts({GraphCreationOptions::Undirected});
  CinderGraph<int, int> graph(opts);

  for (int i = 1; i <= 30; ++i) {
    graph.addVertex(i);
  }

  const int num_reader_threads = 6;
  const int num_writer_threads = 4;
  const int operations_per_thread = 500;

  std::vector<std::thread> threads;
  std::atomic<int> read_operations{0};
  std::atomic<int> write_operations{0};
  std::atomic<bool> test_failed{false};
  std::atomic<bool> stop_test{false};

  for (int i = 0; i < num_reader_threads; ++i) {
    threads.emplace_back([&]() {
      try {
        while (!stop_test &&
               read_operations < operations_per_thread * num_reader_threads) {
          size_t vertices = graph.numVertices();
          size_t edges = graph.numEdges();

          if (vertices != 30) {
            test_failed = true;
            break;
          }

          if (read_operations % 50 == 0) {
            std::string stats = graph.getGraphStatistics();
            if (stats.empty() || extractValue(stats, "Vertices: ") != 30) {
              test_failed = true;
              break;
            }
          }

          read_operations++;
          std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
      } catch (...) {
        test_failed = true;
      }
    });
  }

  for (int i = 0; i < num_writer_threads; ++i) {
    threads.emplace_back([&, i]() {
      try {
        std::mt19937 gen(i * 2000);
        std::uniform_int_distribution<> vertex_dist(1, 30);
        std::uniform_int_distribution<> weight_dist(1, 100);

        while (!stop_test &&
               write_operations < operations_per_thread * num_writer_threads) {
          int v1 = vertex_dist(gen);
          int v2 = vertex_dist(gen);

          try {
            graph.addEdge(v1, v2, weight_dist(gen));
            write_operations++;
          } catch (...) {
          }

          std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
      } catch (...) {
        test_failed = true;
      }
    });
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  stop_test = true;

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_FALSE(test_failed);
  EXPECT_GT(read_operations.load(), 0);
  EXPECT_GT(write_operations.load(), 0);
  EXPECT_EQ(graph.numVertices(), 30);
  std::string final_stats = graph.getGraphStatistics();
  EXPECT_FALSE(final_stats.empty());
  EXPECT_EQ(extractValue(final_stats, "Vertices: "), 30);
}
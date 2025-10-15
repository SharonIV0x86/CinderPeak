// Copyright (c) 2025 CinderPeak Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Copyright (c) 2025 CinderPeak Contributors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>
#include "CinderPeak.hpp"

using namespace CinderPeak;

TEST(AdjacencyListTest, HasVertexTest) {
    // Create a graph with string vertices and int edge weights
    CinderGraph<std::string, int> graph;
    
    // Test with a vertex that doesn't exist
    EXPECT_FALSE(graph.hasVertex("none"));
    
    // Add a vertex and test again
    graph.addVertex("A");
    EXPECT_TRUE(graph.hasVertex("A"));
    
    // Test with a different vertex that wasn't added
    EXPECT_FALSE(graph.hasVertex("B"));
    
    // Add another vertex and test
    graph.addVertex("B");
    EXPECT_TRUE(graph.hasVertex("B"));
    
    // Test with a non-existent vertex
    EXPECT_FALSE(graph.hasVertex("C"));
}

class HasVertexConcurrentTest : public ::testing::Test {
protected:
    CinderPeak::CinderGraph<int, int> graph;
    const int kNumVertices = 1000;
    const int kNumThreads = 10;
    const int kChecksPerThread = 1000;
    std::atomic<int> successCount{0};
    std::atomic<int> falseNegatives{0};
    std::atomic<int> falsePositives{0};

    void SetUp() override {
        // Add vertices from 0 to kNumVertices-1
        for (int i = 0; i < kNumVertices; ++i) {
            graph.addVertex(i);
        }
    }
};

TEST_F(HasVertexConcurrentTest, ConcurrentHasVertexReads) {
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};

    // Reader threads
    for (int t = 0; t < kNumThreads; ++t) {
        threads.emplace_back([this, t, &stop]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(-kNumVertices, 2 * kNumVertices - 1);
            
            while (!stop) {
                int vertex = dis(gen);
                bool exists = (vertex >= 0 && vertex < kNumVertices);
                bool result = graph.hasVertex(vertex);
                
                if (exists && !result) {
                    falseNegatives++;
                } else if (!exists && result) {
                    falsePositives++;
                } else {
                    successCount++;
                }
            }
        });
    }

    // Let the test run for a short duration
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Signal threads to stop
    stop = true;
    
    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Verify results
    EXPECT_EQ(0, falseNegatives.load()) << "Incorrect false negatives detected";
    EXPECT_EQ(0, falsePositives.load()) << "Incorrect false positives detected";
    EXPECT_GT(successCount.load(), 0) << "No successful checks were performed";
}

TEST_F(HasVertexConcurrentTest, MixedReadWriteOperations) {
    std::vector<std::thread> threads;
    std::atomic<bool> stop{false};

    // Reader threads
    for (int t = 0; t < kNumThreads / 2; ++t) {
        threads.emplace_back([this, t, &stop]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, kNumVertices * 2 - 1);
            
            while (!stop) {
                int vertex = dis(gen);
                (void)graph.hasVertex(vertex);
            }
        });
    }

    // Writer threads (adding/removing vertices)
    for (int t = 0; t < kNumThreads / 2; ++t) {
        threads.emplace_back([this, t, &stop]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 1);
            std::uniform_int_distribution<> vertex_dis(kNumVertices, 2 * kNumVertices - 1);
            
            while (!stop) {
                int vertex = vertex_dis(gen);
                if (dis(gen) == 0) {
                    graph.addVertex(vertex);
                } else {
                    graph.removeVertex(vertex);
                }
                std::this_thread::yield();
            }
        });
    }

    // Let the test run for a short duration
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Signal threads to stop
    stop = true;
    
    // Wait for all threads to finish
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // The test passes if it doesn't crash or deadlock
    EXPECT_TRUE(true);
}

TEST(AdjacencyListTest, HasVertexWithPrimitiveType) {
    // Test with integer vertices
    CinderGraph<int, int> intGraph;
    
    // Test with a vertex that doesn't exist
    EXPECT_FALSE(intGraph.hasVertex(1));
    
    // Add a vertex and test
    intGraph.addVertex(1);
    EXPECT_TRUE(intGraph.hasVertex(1));
    
    // Test with a different vertex
    EXPECT_FALSE(intGraph.hasVertex(2));
}

TEST(AdjacencyListTest, HasVertexAfterRemoval) {
    CinderGraph<std::string, int> graph;
    
    // Add and then remove a vertex
    graph.addVertex("A");
    EXPECT_TRUE(graph.hasVertex("A"));
    
    graph.removeVertex("A");
    EXPECT_FALSE(graph.hasVertex("A"));
    
    // Test with a vertex that was never added
    EXPECT_FALSE(graph.hasVertex("B"));
}

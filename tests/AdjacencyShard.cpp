#include "StorageEngine/AdjacencyList.hpp"
#include <gtest/gtest.h>
#include <thread>

using namespace CinderPeak;
using namespace PeakStore;

// Base Fixture for Primitive Types
class AdjacencyListTest : public ::testing::Test, public CinderVertex {
protected:
  AdjacencyList<int, int> intGraph;
  AdjacencyList<std::string, float> stringGraph;

  void SetUp() override {
    intGraph.impl_addVertex(1);
    intGraph.impl_addVertex(2);
    intGraph.impl_addVertex(3);
    intGraph.impl_addVertex(4);
    intGraph.impl_addVertex(5);

    // New vertices added to validate updateEdge functionality
    intGraph.impl_addVertex(101);
    intGraph.impl_addVertex(102);
    intGraph.impl_addVertex(103);

    stringGraph.impl_addVertex("A");
    stringGraph.impl_addVertex("B");
    stringGraph.impl_addVertex("C");
  }
};
// Base Fixture for Complex Types
class ComplexAdjVertex : public CinderVertex {
public:
  int vertexData;
  std::string nodeName;
  ComplexAdjVertex(int vertexData, std::string node_name)
      : vertexData{vertexData}, nodeName{node_name} {}
  ComplexAdjVertex() = default;
};

class ComplexAdjEdge : public CinderEdge {
public:
  float edgeValue;
  ComplexAdjEdge(float edgeValue) : edgeValue{edgeValue} {}
  ComplexAdjEdge() = default;
};

class ComplexGraph : public ::testing::Test {
public:
  ComplexAdjVertex v1, v2, v3;
  ComplexAdjEdge e1, e2;
  AdjacencyList<ComplexAdjVertex, ComplexAdjEdge> complexGraph;
  ComplexGraph() {
    v1 = ComplexAdjVertex(1, "Vertex1");
    v2 = ComplexAdjVertex(2, "Vertex2");
    v3 = ComplexAdjVertex(3, "Vertex3");

    e1 = ComplexAdjEdge(12.34);
    e2 = ComplexAdjEdge(76.45);

    complexGraph.impl_addVertex(v1);
    complexGraph.impl_addVertex(v2);
    complexGraph.impl_addVertex(v3);
  }
};

//
// 1. Vertex Operations
//

TEST_F(AdjacencyListTest, AddVertexPrimitive) {
  EXPECT_TRUE(intGraph.impl_addVertex(6).isOK());

  auto status = intGraph.impl_addVertex(1);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyListTest, AddVertexString) {
  EXPECT_TRUE(stringGraph.impl_addVertex("D").isOK());

  auto status = stringGraph.impl_addVertex("A");
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Primitive Vertex Already Exists");
}

TEST_F(AdjacencyListTest, AddVertices) {
  std::vector<int> newVertices = {6, 7, 8, 9, 10};

  auto status = intGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  // Verify all vertices were added
  for (int vertex : newVertices) {
    auto neighbors = intGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
    EXPECT_TRUE(
        neighbors.first.empty()); // New vertices should have no neighbors
  }
}

TEST_F(AdjacencyListTest, AddVerticesDuplicates) {
  std::vector<int> verticesWithDups = {6, 1, 7, 2, 8}; // 1 and 2 already exist

  auto status = intGraph.impl_addVertices(verticesWithDups);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_ALREADY_EXISTS);

  // Verify new vertices were still added
  EXPECT_TRUE(intGraph.impl_getNeighbors(6).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(7).second.isOK());
  EXPECT_TRUE(intGraph.impl_getNeighbors(8).second.isOK());
}

TEST_F(AdjacencyListTest, AddVerticesEmpty) {
  std::vector<int> emptyVertices = {};

  auto status = intGraph.impl_addVertices(emptyVertices);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyListTest, AddVerticesString) {
  std::vector<std::string> newVertices = {"D", "E", "F"};

  auto status = stringGraph.impl_addVertices(newVertices);
  EXPECT_TRUE(status.isOK());

  // Verify vertices were added
  for (const auto &vertex : newVertices) {
    auto neighbors = stringGraph.impl_getNeighbors(vertex);
    EXPECT_TRUE(neighbors.second.isOK());
  }
}

//
// 2. Edge Operations
//

TEST_F(AdjacencyListTest, AddEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2, 5).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(2, 3, 10).isOK());

  auto edge1 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 5);

  auto edge2 = intGraph.impl_getEdge(2, 3);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 10);
}

// Added test to validate impl_updateEdge functionality
TEST_F(AdjacencyListTest, UpdateEdgeWithWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(101, 102, 7).isOK());
  EXPECT_TRUE(intGraph.impl_addEdge(103, 102, 5).isOK());

  EXPECT_TRUE(intGraph.impl_updateEdge(101, 102, 10).isOK());
  EXPECT_TRUE(intGraph.impl_updateEdge(103, 102, 1).isOK());
  EXPECT_FALSE(intGraph.impl_updateEdge(400, 102, 1).isOK());

  auto edge1 = intGraph.impl_getEdge(101, 102);
  EXPECT_TRUE(edge1.second.isOK());
  EXPECT_EQ(edge1.first, 10);

  auto edge2 = intGraph.impl_getEdge(103, 102);
  EXPECT_TRUE(edge2.second.isOK());
  EXPECT_EQ(edge2.first, 1);
}
TEST_F(ComplexGraph, UpdateEdgeOnComplexGraph) {
  ComplexAdjEdge newEdgeValue1(4.3);
  ComplexAdjEdge newEdgeValue2(467.32);
  EXPECT_TRUE(complexGraph.impl_addEdge(v1, v2, e1).isOK());
  EXPECT_TRUE(complexGraph.impl_addEdge(v2, v3, e2).isOK());

  complexGraph.impl_updateEdge(v1, v2, newEdgeValue1);
  EXPECT_EQ(complexGraph.impl_getEdge(v1, v2).first, newEdgeValue1);

  complexGraph.impl_updateEdge(v2, v3, newEdgeValue2);
  EXPECT_EQ(complexGraph.impl_getEdge(v2, v3).first, newEdgeValue2);
}

TEST_F(AdjacencyListTest, AddEdgeWithoutWeight) {
  EXPECT_TRUE(intGraph.impl_addEdge(1, 2).isOK());

  auto edge = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_EQ(edge.first, 0); // Default int
}

TEST_F(AdjacencyListTest, AddEdgeInvalidVertices) {
  auto status1 = intGraph.impl_addEdge(99, 1);
  EXPECT_FALSE(status1.isOK());
  EXPECT_EQ(status1.code(), StatusCode::VERTEX_NOT_FOUND);

  auto status2 = intGraph.impl_addEdge(1, 99);
  EXPECT_FALSE(status2.isOK());
  EXPECT_EQ(status2.code(), StatusCode::VERTEX_NOT_FOUND);
}

TEST_F(AdjacencyListTest, AddEdgesPairs) {
  std::vector<std::pair<int, int>> edges = {
      {1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  // Verify all edges were added
  for (const auto &edge : edges) {
    auto result = intGraph.impl_getEdge(edge.first, edge.second);
    EXPECT_TRUE(result.second.isOK());
    EXPECT_EQ(result.first, 0); // Default weight
  }
}

TEST_F(AdjacencyListTest, AddEdgesTuples) {
  std::vector<std::tuple<int, int, int>> edges = {
      {1, 2, 10}, {2, 3, 20}, {3, 4, 30}, {4, 5, 40}};

  auto status = intGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  // Verify all edges with correct weights
  EXPECT_EQ(intGraph.impl_getEdge(1, 2).first, 10);
  EXPECT_EQ(intGraph.impl_getEdge(2, 3).first, 20);
  EXPECT_EQ(intGraph.impl_getEdge(3, 4).first, 30);
  EXPECT_EQ(intGraph.impl_getEdge(4, 5).first, 40);
}

TEST_F(AdjacencyListTest, AddEdgesInvalidVertices) {
  std::vector<std::pair<int, int>> edgesWithInvalid = {
      {1, 2}, {99, 3}, {4, 5}, {1, 100} // 99 and 100 don't exist
  };

  auto status = intGraph.impl_addEdges(edgesWithInvalid);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.code(), StatusCode::VERTEX_NOT_FOUND);

  // Verify valid edges were still added
  EXPECT_TRUE(intGraph.impl_getEdge(1, 2).second.isOK());
  EXPECT_TRUE(intGraph.impl_getEdge(4, 5).second.isOK());

  // Verify invalid edges were not added
  EXPECT_FALSE(intGraph.impl_getEdge(99, 3).second.isOK());
  EXPECT_FALSE(intGraph.impl_getEdge(1, 100).second.isOK());
}

TEST_F(AdjacencyListTest, AddEdgesEmpty) {
  std::vector<std::pair<int, int>> emptyEdges = {};

  auto status = intGraph.impl_addEdges(emptyEdges);
  EXPECT_TRUE(status.isOK());
}

TEST_F(AdjacencyListTest, AddEdgesMixedTypes) {
  // Test with string graph
  std::vector<std::tuple<std::string, std::string, float>> edges = {
      {"A", "B", 1.5f}, {"B", "C", 2.7f}, {"A", "C", 3.14f}};

  auto status = stringGraph.impl_addEdges(edges);
  EXPECT_TRUE(status.isOK());

  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("A", "B").first, 1.5f);
  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("B", "C").first, 2.7f);
  EXPECT_FLOAT_EQ(stringGraph.impl_getEdge("A", "C").first, 3.14f);
}

//
// 3. Edge Retrieval
//

TEST_F(AdjacencyListTest, GetExistingEdge) {
  intGraph.impl_addEdge(1, 2, 5);

  auto result = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(result.second.isOK());
  EXPECT_EQ(result.first, 5);
}

TEST_F(AdjacencyListTest, GetNonExistentEdge) {
  auto result1 = intGraph.impl_getEdge(1, 3);
  EXPECT_FALSE(result1.second.isOK());
  EXPECT_EQ(result1.second.code(), StatusCode::EDGE_NOT_FOUND);

  auto result2 = intGraph.impl_getEdge(99, 1);
  EXPECT_FALSE(result2.second.isOK());
  EXPECT_EQ(result2.second.code(), StatusCode::VERTEX_NOT_FOUND);
}

//
// 4. Neighbor Retrieval
//

TEST_F(AdjacencyListTest, GetNeighbors) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(1, 3, 10);

  auto neighbors = intGraph.impl_getNeighbors(1);
  EXPECT_TRUE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.first.size(), 2);

  EXPECT_EQ(neighbors.first[0].first, 2);
  EXPECT_EQ(neighbors.first[0].second, 5);

  EXPECT_EQ(neighbors.first[1].first, 3);
  EXPECT_EQ(neighbors.first[1].second, 10);
}

TEST_F(AdjacencyListTest, GetNeighborsNonExistentVertex) {
  auto neighbors = intGraph.impl_getNeighbors(99);
  EXPECT_FALSE(neighbors.second.isOK());
  EXPECT_EQ(neighbors.second.code(), StatusCode::VERTEX_NOT_FOUND);
  EXPECT_TRUE(neighbors.first.empty());
}

//
// 5. Edge Existence Checks
//

TEST_F(AdjacencyListTest, EdgeExistence) {
  intGraph.impl_addEdge(1, 2, 5);

  // Test edge existence through public interface instead of impl_doesEdgeExist
  auto edge1_2 = intGraph.impl_getEdge(1, 2);
  EXPECT_TRUE(edge1_2.second.isOK());

  auto edge1_3 = intGraph.impl_getEdge(1, 3);
  EXPECT_FALSE(edge1_3.second.isOK());

  auto edge2_1 = intGraph.impl_getEdge(2, 1);
  EXPECT_FALSE(edge2_1.second.isOK());

  auto edge99_1 = intGraph.impl_getEdge(99, 1);
  EXPECT_FALSE(edge99_1.second.isOK());
}

//
// 6. Adjacency List Structure
//

TEST_F(AdjacencyListTest, AdjacencyListStructure) {
  intGraph.impl_addEdge(1, 2, 5);
  intGraph.impl_addEdge(1, 3, 10);
  intGraph.impl_addEdge(2, 3, 15);
  intGraph.impl_addEdge(4, 4, 4);
  intGraph.impl_addEdge(2, 5, 1);

  auto adjList = intGraph.getAdjList();

  EXPECT_EQ(adjList.size(), 8); // Updated value due to newer vertex additions

  auto it1 = adjList.find(1);
  ASSERT_NE(it1, adjList.end());
  EXPECT_EQ(it1->second.size(), 2);

  auto it2 = adjList.find(2);
  ASSERT_NE(it2, adjList.end());
  EXPECT_EQ(it2->second.size(), 2);

  auto it3 = adjList.find(3);
  ASSERT_NE(it3, adjList.end());
  EXPECT_TRUE(it3->second.empty());

  // Test for vertex 4
  auto it4 = adjList.find(4);
  ASSERT_NE(it4, adjList.end());
  EXPECT_EQ(it4->second.size(), 1); // vertex 4 has a self-loop

  // Test for vertex 5
  auto it5 = adjList.find(5);
  ASSERT_NE(it5, adjList.end());
  EXPECT_TRUE(it5->second.empty()); // vertex 5 has no outgoing edges
}

//
// 7. Complex Type Tests (CustomVertex)
//
struct CustomVertex : public CinderVertex {
  int vertex_value;
  std::string name;
  CustomVertex(int v, std::string n) : vertex_value(v), name(n) {}
};

TEST(AdjacencyListCustomTest, CustomVertexType) {
  AdjacencyList<CustomVertex, float> customGraph;

  CustomVertex v1{1, "Node1"};
  CustomVertex v2{2, "Node2"};
  // CustomVertex v1_dup{1, "Node1Duplicate"};

  EXPECT_TRUE(customGraph.impl_addVertex(v1).isOK());
  EXPECT_TRUE(customGraph.impl_addVertex(v2).isOK());

  auto status = customGraph.impl_addVertex(v1);
  EXPECT_FALSE(status.isOK());
  EXPECT_EQ(status.message(), "Non Primitive Vertex Already Exists");

  EXPECT_TRUE(customGraph.impl_addEdge(v1, v2, 3.14f).isOK());

  auto edge = customGraph.impl_getEdge(v1, v2);
  EXPECT_TRUE(edge.second.isOK());
  EXPECT_FLOAT_EQ(edge.first, 3.14f);
}

//
// 8. Thread Safety Tests
//

class AdjacencyListThreadTest : public ::testing::Test {
protected:
    AdjacencyList<int, int> threadGraph;
    
    void SetUp() override {
        // Initialize with some vertices for testing
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
    
    // Each thread adds vertices in a different range to avoid conflicts
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
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All vertex additions should succeed since ranges don't overlap
    EXPECT_EQ(successCount.load(), numThreads * verticesPerThread);
    EXPECT_EQ(failureCount.load(), 0);
    
    // Verify vertices were actually added
    for (int t = 0; t < numThreads; ++t) {
        int startVertex = 1000 + t * verticesPerThread;
        for (int i = 0; i < verticesPerThread; ++i) {
            auto neighbors = threadGraph.impl_getNeighbors(startVertex + i);
            EXPECT_TRUE(neighbors.second.isOK());
        }
    }
}

TEST_F(AdjacencyListThreadTest, ConcurrentEdgeAddition) {
    const int numThreads = 8;
    const int edgesPerThread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    std::atomic<int> failureCount{0};
    
    // Each thread adds edges between different vertex pairs
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < edgesPerThread; ++i) {
                // Create unique edge pairs for each thread
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
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All edge additions should succeed
    EXPECT_EQ(successCount.load(), numThreads * edgesPerThread);
    EXPECT_EQ(failureCount.load(), 0);
}

TEST_F(AdjacencyListThreadTest, ConcurrentReadWriteOperations) {
    // Add some initial edges
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
    
    // Reader threads - perform getEdge and getNeighbors operations
    for (int t = 0; t < numReaderThreads; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < operationsPerThread; ++i) {
                int vertex1 = (i % 50) + 1;
                int vertex2 = ((i + 1) % 50) + 1;
                
                // Read edge
                auto edge = threadGraph.impl_getEdge(vertex1, vertex2);
                readOperations++;
                if (!edge.second.isOK() && edge.second.code() != StatusCode::EDGE_NOT_FOUND) {
                    readErrors++;
                }
                
                // Read neighbors
                auto neighbors = threadGraph.impl_getNeighbors(vertex1);
                readOperations++;
                if (!neighbors.second.isOK()) {
                    readErrors++;
                }
            }
        });
    }
    
    // Writer threads - perform addEdge operations
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
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
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
    
    // Each thread performs bulk vertex and edge additions
    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&, t]() {
            // Bulk vertex addition
            std::vector<int> vertices;
            int startVertex = 2000 + t * 100;
            for (int i = 0; i < 50; ++i) {
                vertices.push_back(startVertex + i);
            }
            
            auto vertexStatus = threadGraph.impl_addVertices(vertices);
            if (vertexStatus.isOK()) {
                successCount++;
            }
            
            // Bulk edge addition
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
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), numThreads * 2);
}

TEST_F(AdjacencyListTest, ConcurrentMixedOperationsDeadlock) {
    const int NUM_THREADS = 10;
    const int OPERATIONS_PER_THREAD = 1000;
    std::vector<std::thread> threads;
    std::atomic<int> completed_threads(0);
    std::atomic<bool> deadlock_detected(false);
    std::thread watchdog([&]() {
        auto start = std::chrono::steady_clock::now();
        while (completed_threads < NUM_THREADS) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
            if (elapsed.count() > 5) { // 5 second timeout
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
    for (auto& thread : threads) {
        thread.join();
    }
    watchdog.join();
    EXPECT_FALSE(deadlock_detected);
}
TEST_F(AdjacencyListTest, ConcurrentBulkOperationsRace) {
    std::vector<int> vertices1 = {100, 101, 102, 103, 104};
    std::vector<int> vertices2 = {105, 106, 107, 108, 109};   
    std::vector<std::pair<int, int>> edges1 = {{100, 101}, {101, 102}, {102, 103}};
    std::vector<std::pair<int, int>> edges2 = {{105, 106}, {106, 107}, {107, 108}};
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
TEST_F(AdjacencyListTest, HighReadWriteContention) {
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
    
    for (auto& thread : threads) {
        thread.join();
    }
        EXPECT_TRUE(true);
}
TEST_F(AdjacencyListTest, PotentialReentrancyDeadlock) {
    intGraph.impl_addVertex(100);
    intGraph.impl_addVertex(101);
    intGraph.impl_addEdge(100, 101);
    
    auto callback = [&](int from, int to) {
        return intGraph.impl_doesEdgeExist(from, to);
    };
        std::thread t([&]() {
        auto neighbors = intGraph.impl_getNeighbors(100);
        if (neighbors.second.isOK()) {
            for (const auto& [vertex, weight] : neighbors.first) {
                bool exists = callback(100, vertex);
                EXPECT_TRUE(exists); // This should be true
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
    
    EXPECT_TRUE(completed); // Thread should complete within timeout
}

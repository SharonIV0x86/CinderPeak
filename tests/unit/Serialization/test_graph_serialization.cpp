#include "CinderGraph.hpp"
#include "StorageEngine/GraphSerialization.hpp"
#include <filesystem>
#include <gtest/gtest.h>

using namespace CinderPeak;

class GraphSerializationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test directory if it doesn't exist
        std::filesystem::create_directories("test_output");
    }

    void TearDown() override
    {
        // Clean up test files
        std::filesystem::remove_all("test_output");
    }
};

// Test serialization and deserialization of a simple weighted graph with
// integers
TEST_F(GraphSerializationTest, SimpleWeightedIntGraph)
{
    // Create a graph
    GraphCreationOptions options({GraphCreationOptions::Directed,
                                  GraphCreationOptions::SelfLoops});
    CinderGraph<int, int> graph(options);

    // Add vertices
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    // Add edges
    graph.addEdge(1, 2, 10);
    graph.addEdge(2, 3, 20);
    graph.addEdge(1, 3, 30);

    // Save graph
    std::string filepath = "test_output/simple_int_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);
    EXPECT_TRUE(save_result.status.isOK());

    // Create a new graph and load
    CinderGraph<int, int> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);
    EXPECT_TRUE(load_result.status.isOK());

    // Verify loaded graph
    EXPECT_EQ(loaded_graph.numVertices(), 3);
    EXPECT_EQ(loaded_graph.numEdges(), 3);

    // Verify edges
    auto [weight1, found1] = loaded_graph.getEdge(1, 2);
    EXPECT_TRUE(found1);
    EXPECT_EQ(*weight1, 10);

    auto [weight2, found2] = loaded_graph.getEdge(2, 3);
    EXPECT_TRUE(found2);
    EXPECT_EQ(*weight2, 20);

    auto [weight3, found3] = loaded_graph.getEdge(1, 3);
    EXPECT_TRUE(found3);
    EXPECT_EQ(*weight3, 30);
}

// Test serialization and deserialization of a string graph
TEST_F(GraphSerializationTest, StringGraph)
{
    GraphCreationOptions options({GraphCreationOptions::Directed});
    CinderGraph<std::string, double> graph(options);

    // Add vertices
    graph.addVertex("A");
    graph.addVertex("B");
    graph.addVertex("C");
    graph.addVertex("D");

    // Add edges
    graph.addEdge("A", "B", 1.5);
    graph.addEdge("B", "C", 2.5);
    graph.addEdge("C", "D", 3.5);
    graph.addEdge("A", "D", 4.5);

    // Save graph
    std::string filepath = "test_output/string_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    // Load graph
    CinderGraph<std::string, double> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    // Verify
    EXPECT_EQ(loaded_graph.numVertices(), 4);
    EXPECT_EQ(loaded_graph.numEdges(), 4);

    auto [weight1, found1] = loaded_graph.getEdge("A", "B");
    EXPECT_TRUE(found1);
    EXPECT_DOUBLE_EQ(*weight1, 1.5);
}

// Test serialization of undirected graph
TEST_F(GraphSerializationTest, UndirectedGraph)
{
    GraphCreationOptions options({GraphCreationOptions::Undirected});
    CinderGraph<int, float> graph(options);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);

    graph.addEdge(1, 2, 1.1f);
    graph.addEdge(2, 3, 2.2f);

    std::string filepath = "test_output/undirected_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, float> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 3);
    // Undirected edges are stored as bidirectional
    EXPECT_TRUE(loaded_graph.hasVertex(1));
    EXPECT_TRUE(loaded_graph.hasVertex(2));
    EXPECT_TRUE(loaded_graph.hasVertex(3));
}

// Test empty graph serialization
TEST_F(GraphSerializationTest, EmptyGraph)
{
    CinderGraph<int, int> graph;

    std::string filepath = "test_output/empty_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, int> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 0);
    EXPECT_EQ(loaded_graph.numEdges(), 0);
}

// Test single vertex graph
TEST_F(GraphSerializationTest, SingleVertexGraph)
{
    CinderGraph<int, int> graph;
    graph.addVertex(42);

    std::string filepath = "test_output/single_vertex_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, int> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 1);
    EXPECT_EQ(loaded_graph.numEdges(), 0);
    EXPECT_TRUE(loaded_graph.hasVertex(42));
}

// Test graph with self-loops
TEST_F(GraphSerializationTest, GraphWithSelfLoops)
{
    GraphCreationOptions options({GraphCreationOptions::Directed,
                                  GraphCreationOptions::SelfLoops});
    CinderGraph<int, int> graph(options);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 1, 100); // Self-loop
    graph.addEdge(1, 2, 10);
    graph.addEdge(2, 2, 200); // Self-loop

    std::string filepath = "test_output/self_loop_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, int> loaded_graph(options);
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 2);
    EXPECT_EQ(loaded_graph.numEdges(), 3);

    auto [weight, found] = loaded_graph.getEdge(1, 1);
    EXPECT_TRUE(found);
    EXPECT_EQ(*weight, 100);
}

// Test invalid file path
TEST_F(GraphSerializationTest, InvalidFilePath)
{
    CinderGraph<int, int> graph;
    graph.addVertex(1);

    // Try to save to invalid path
    std::string invalid_path = "/invalid/path/that/does/not/exist/graph.json";
    auto save_result = graph.save(invalid_path);
    EXPECT_FALSE(save_result.success);
}

// Test loading from non-existent file
TEST_F(GraphSerializationTest, LoadNonExistentFile)
{
    CinderGraph<int, int> graph;

    std::string filepath = "test_output/nonexistent_graph.json";
    auto load_result = graph.load(filepath);
    EXPECT_FALSE(load_result.success);
}

// Test CinderVertex serialization
TEST_F(GraphSerializationTest, CinderVertexGraph)
{
    GraphCreationOptions options({GraphCreationOptions::Directed});
    CinderGraph<CinderVertex, int> graph(options);

    CinderVertex v1("Vertex1");
    CinderVertex v2("Vertex2");
    CinderVertex v3("Vertex3");

    graph.addVertex(v1);
    graph.addVertex(v2);
    graph.addVertex(v3);

    graph.addEdge(v1, v2, 10);
    graph.addEdge(v2, v3, 20);

    std::string filepath = "test_output/cinder_vertex_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<CinderVertex, int> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 3);
    EXPECT_EQ(loaded_graph.numEdges(), 2);
}

// Test parallel edges support
TEST_F(GraphSerializationTest, ParallelEdgesGraph)
{
    GraphCreationOptions options({GraphCreationOptions::Directed,
                                  GraphCreationOptions::ParallelEdges});
    CinderGraph<int, int> graph(options);

    graph.addVertex(1);
    graph.addVertex(2);
    graph.addEdge(1, 2, 10);
    graph.addEdge(1, 2, 20); // Parallel edge with different weight

    std::string filepath = "test_output/parallel_edges_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, int> loaded_graph(options);
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 2);
    EXPECT_EQ(loaded_graph.numEdges(), 2);
}

// Test large graph
TEST_F(GraphSerializationTest, LargeGraph)
{
    CinderGraph<int, int> graph;

    // Create a graph with 100 vertices
    for (int i = 0; i < 100; ++i)
    {
        graph.addVertex(i);
    }

    // Add edges forming a chain
    for (int i = 0; i < 99; ++i)
    {
        graph.addEdge(i, i + 1, i * 10);
    }

    std::string filepath = "test_output/large_graph.json";
    auto save_result = graph.save(filepath);
    EXPECT_TRUE(save_result.success);

    CinderGraph<int, int> loaded_graph;
    auto load_result = loaded_graph.load(filepath);
    EXPECT_TRUE(load_result.success);

    EXPECT_EQ(loaded_graph.numVertices(), 100);
    EXPECT_EQ(loaded_graph.numEdges(), 99);
}

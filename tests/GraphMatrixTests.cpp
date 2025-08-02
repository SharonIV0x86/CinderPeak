#include <gtest/gtest.h>
#include "GraphMatrix.hpp"
#include "StorageEngine/Utils.hpp"

using namespace CinderPeak;

class GraphMatrixTest : public ::testing::Test {
protected:
    GraphMatrix<int, int> intGraph;
    GraphMatrix<std::string, double> stringGraph;

    void SetUp() override {
        // Setup basic vertices for testing
        intGraph.addVertex(1);
        intGraph.addVertex(2);
        intGraph.addVertex(3);
        
        stringGraph.addVertex("A");
        stringGraph.addVertex("B");
        stringGraph.addVertex("C");
    }
};

TEST_F(GraphMatrixTest, AddVertexTest) {
    // Test adding a new vertex
    EXPECT_NO_THROW(intGraph.addVertex(4));
    EXPECT_NO_THROW(stringGraph.addVertex("D"));
}

TEST_F(GraphMatrixTest, AddUnweightedEdgeTest) {
    // Create unweighted graph
    GraphCreationOptions unweightedOptions({GraphCreationOptions::Unweighted});
    GraphMatrix<int, int> unweightedGraph(unweightedOptions);
    
    unweightedGraph.addVertex(1);
    unweightedGraph.addVertex(2);
    
    EXPECT_NO_THROW(unweightedGraph.addEdge(1, 2));
}

TEST_F(GraphMatrixTest, AddWeightedEdgeTest) {
    // Create weighted graph
    GraphCreationOptions weightedOptions({GraphCreationOptions::Weighted});
    GraphMatrix<int, int> weightedGraph(weightedOptions);
    
    weightedGraph.addVertex(1);
    weightedGraph.addVertex(2);
    
    EXPECT_NO_THROW(weightedGraph.addEdge(1, 2, 5));
}

TEST_F(GraphMatrixTest, GetEdgeTest) {
    // Create weighted graph and test edge retrieval
    GraphCreationOptions weightedOptions({GraphCreationOptions::Weighted});
    GraphMatrix<int, int> weightedGraph(weightedOptions);
    
    weightedGraph.addVertex(1);
    weightedGraph.addVertex(2);
    weightedGraph.addEdge(1, 2, 10);
    
    EXPECT_EQ(weightedGraph.getEdge(1, 2), 10);
}

TEST_F(GraphMatrixTest, EdgeAccessorTest) {
    // Create weighted graph and test edge accessor syntax
    GraphCreationOptions weightedOptions({GraphCreationOptions::Weighted});
    GraphMatrix<int, int> weightedGraph(weightedOptions);
    
    weightedGraph.addVertex(1);
    weightedGraph.addVertex(2);
    
    // Test assignment through accessor
    EXPECT_NO_THROW(weightedGraph[1][2] = 15);
    
    // Test reading through accessor
    EXPECT_EQ(static_cast<int>(weightedGraph[1][2]), 15);
}

TEST_F(GraphMatrixTest, VisualizationTest) {
    // Test that visualization method can be called without throwing
    EXPECT_NO_THROW(intGraph.visualize());
    EXPECT_NO_THROW(stringGraph.visualize());
}

// Test with custom vertex types
struct CustomVertex {
    int id;
    std::string name;
    
    CustomVertex(int i, const std::string& n) : id(i), name(n) {}
    
    bool operator==(const CustomVertex& other) const {
        return id == other.id && name == other.name;
    }
};

// Custom hash function for CustomVertex
namespace std {
    template<>
    struct hash<CustomVertex> {
        size_t operator()(const CustomVertex& v) const {
            return hash<int>()(v.id) ^ (hash<string>()(v.name) << 1);
        }
    };
}

TEST(GraphMatrixCustomTest, CustomVertexTest) {
    GraphMatrix<CustomVertex, double> customGraph;
    
    CustomVertex v1{1, "Node1"};
    CustomVertex v2{2, "Node2"};
    
    EXPECT_NO_THROW(customGraph.addVertex(v1));
    EXPECT_NO_THROW(customGraph.addVertex(v2));
}

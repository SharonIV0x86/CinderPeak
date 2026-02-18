#include <gtest/gtest.h>
#include "CinderPeak.hpp"
#include <vector>
#include <string>
#include <set>

using namespace CinderPeak;
using namespace CinderPeak::Algorithms;

class GraphAlgorithmsTest : public ::testing::Test {
protected:
    CinderGraph<std::string, int> weightedGraph;
    CinderGraph<std::string, Unweighted> unweightedGraph;

    void SetUp() override {
        // Setup a standard weighted graph for testing
        // A --(1)--> B --(2)--> C
        // A --(4)--> C
        // B --(1)--> D
        // C --(3)--> D
        weightedGraph.addVertex("A");
        weightedGraph.addVertex("B");
        weightedGraph.addVertex("C");
        weightedGraph.addVertex("D");

        weightedGraph.addEdge("A", "B", 1);
        weightedGraph.addEdge("B", "C", 2);
        weightedGraph.addEdge("A", "C", 4);
        weightedGraph.addEdge("B", "D", 1);
        weightedGraph.addEdge("C", "D", 3);

        // Setup an unweighted graph for search tests
        unweightedGraph.addVertex("A");
        unweightedGraph.addVertex("B");
        unweightedGraph.addVertex("C");
        unweightedGraph.addVertex("D");

        unweightedGraph.addEdge("A", "B");
        unweightedGraph.addEdge("B", "C");
        unweightedGraph.addEdge("C", "A");
        unweightedGraph.addEdge("B", "D");
    }
};

TEST_F(GraphAlgorithmsTest, BFSTest) {
    std::vector<std::string> visited;
    bfs(unweightedGraph, std::string("A"), [&](const std::string& v) {
        visited.push_back(v);
    });

    // BFS should visit A, then B, then C and D
    ASSERT_EQ(visited.size(), 4);
    EXPECT_EQ(visited[0], "A");
    EXPECT_EQ(visited[1], "B");
    // C and D could be in any order depending on internal storage, but we know they come after B
    std::set<std::string> nextLevel = {visited[2], visited[3]};
    EXPECT_TRUE(nextLevel.count("C"));
    EXPECT_TRUE(nextLevel.count("D"));
}

TEST_F(GraphAlgorithmsTest, DFSTest) {
    std::vector<std::string> visited;
    dfs(unweightedGraph, std::string("A"), [&](const std::string& v) {
        visited.push_back(v);
    });

    ASSERT_EQ(visited.size(), 4);
    EXPECT_EQ(visited[0], "A");
    // DFS from A -> B -> (C or D)
    EXPECT_EQ(visited[1], "B");
}

TEST_F(GraphAlgorithmsTest, DijkstraTest) {
    auto result = dijkstra(weightedGraph, std::string("A"));

    EXPECT_EQ(result.distances["A"], 0);
    EXPECT_EQ(result.distances["B"], 1);
    EXPECT_EQ(result.distances["C"], 3); // A->B->C is 1+2=3, A->C is 4
    EXPECT_EQ(result.distances["D"], 2); // A->B->D is 1+1=2
}

TEST_F(GraphAlgorithmsTest, TopologicalSortTest) {
    CinderGraph<std::string, Unweighted> dag;
    dag.addVertex("5");
    dag.addVertex("7");
    dag.addVertex("3");
    dag.addVertex("11");
    dag.addVertex("8");
    dag.addVertex("2");
    dag.addVertex("9");
    dag.addVertex("10");

    dag.addEdge("5", "11");
    dag.addEdge("7", "11");
    dag.addEdge("7", "8");
    dag.addEdge("3", "8");
    dag.addEdge("3", "10");
    dag.addEdge("11", "2");
    dag.addEdge("11", "9");
    dag.addEdge("11", "10");
    dag.addEdge("8", "9");

    auto sorted = topologicalSort(dag);
    ASSERT_EQ(sorted.size(), 8);

    // Verify topological order: for every edge u -> v, u comes before v
    std::unordered_map<std::string, int> pos;
    for (int i = 0; i < sorted.size(); ++i) {
        pos[sorted[i]] = i;
    }

    auto checkEdge = [&](const std::string& u, const std::string& v) {
        EXPECT_LT(pos[u], pos[v]);
    };

    checkEdge("5", "11");
    checkEdge("7", "11");
    checkEdge("7", "8");
    checkEdge("3", "8");
    checkEdge("3", "10");
    checkEdge("11", "2");
    checkEdge("11", "9");
    checkEdge("11", "10");
    checkEdge("8", "9");
}

TEST_F(GraphAlgorithmsTest, PrimMSTTest) {
    CinderGraph<std::string, int> mstGraph;
    mstGraph.addVertex("A");
    mstGraph.addVertex("B");
    mstGraph.addVertex("C");
    mstGraph.addVertex("D");

    // Undirected graph representation (add both ways for MST)
    auto addUndirected = [&](const std::string& u, const std::string& v, int w) {
        mstGraph.addEdge(u, v, w);
        mstGraph.addEdge(v, u, w);
    };

    addUndirected("A", "B", 1);
    addUndirected("B", "C", 3);
    addUndirected("A", "C", 3);
    addUndirected("B", "D", 1);
    addUndirected("C", "D", 6);

    auto mst = primMST(mstGraph);
    
    // MST should have 3 edges (4 vertices - 1)
    ASSERT_EQ(mst.size(), 3);
    
    int totalWeight = 0;
    for (const auto& edge : mst) {
        totalWeight += edge.weight;
    }
    
    // Edges should be (A,B,1), (B,D,1), (A,C,3) or (B,C,3)
    // Total weight = 1 + 1 + 3 = 5
    EXPECT_EQ(totalWeight, 5);
}

TEST_F(GraphAlgorithmsTest, EmptyGraphTest) {
    CinderGraph<std::string, int> emptyGraph;
    auto result = dijkstra(emptyGraph, std::string("A"));
    EXPECT_TRUE(result.distances.empty());

    auto mst = primMST(emptyGraph);
    EXPECT_TRUE(mst.empty());

    CinderGraph<std::string, Unweighted> emptyUnweighted;
    auto sorted = topologicalSort(emptyUnweighted);
    EXPECT_TRUE(sorted.empty());
}

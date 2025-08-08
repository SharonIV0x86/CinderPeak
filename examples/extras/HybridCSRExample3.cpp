#include "StorageEngine/HybridCSR_COO.hpp"
#include "StorageEngine/ErrorCodes.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>

using namespace CinderPeak::PeakStore;

class CustomVertex {
public:
    int id;
    CustomVertex(int i = 0) : id(i) {}
    bool operator==(const CustomVertex &other) const { return id == other.id; }
    friend std::ostream &operator<<(std::ostream &os, const CustomVertex &v) {
        return os << "V" << v.id;
    }
};

// Hash function for CustomVertex
namespace std {
    template<>
    struct hash<CustomVertex> {
        std::size_t operator()(const CustomVertex &v) const { return std::hash<int>()(v.id); }
    };
}

int main() {
    std::cout << "--- HybridCSR_COO Example 3: Custom Vertex and Edge Types ---" << std::endl;

    // Dummy adjacency list with custom vertices
    std::unordered_map<CustomVertex, std::vector<std::pair<CustomVertex, int>>> adjList = {
        {CustomVertex(1), {{CustomVertex(2), 100}}},
        {CustomVertex(2), {{CustomVertex(3), 200}}},
        {CustomVertex(3), {}}
    };

    HybridCSR_COO<CustomVertex, int> hybridGraph;
    hybridGraph.populateFromAdjList(adjList);

    std::cout << "HybridCSR_COO populated with custom vertex types." << std::endl;
    
    // Demonstrate a query using an impl function
    CustomVertex src(1);
    CustomVertex dest(2);
    auto [edge_data, status] = hybridGraph.impl_getEdge(src, dest);

    if (status.isOK()) {
        std::cout << "Edge from " << src << " to " << dest << " exists with weight: " << edge_data << std::endl;
    } else {
        std::cout << "Edge from " << src << " to " << dest << " not found." << std::endl;
        std::cout << "Error: " << status.message() << std::endl;
    }

    return 0;
}

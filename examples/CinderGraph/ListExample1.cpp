#include "CinderPeak.hpp"
using namespace CinderPeak;

class ListVertex : public CinderVertex {
public:
  int data;
  ListVertex(int data) : data{data} {};
  ListVertex() = default;
};

class ListEdge : public CinderEdge {
public:
  float edge_weight;
  ListEdge(float edge_weight) : edge_weight{edge_weight} {};
  ListEdge() = default;
};

int main() {
  // ---------------- Graph 1 (weighted int edges) ----------------
  GraphCreationOptions opts({GraphCreationOptions::Directed});
  CinderGraph<int, int> graph(opts);

  CinderGraph<int, int>::setConsoleLogging(false); // Disabling log display

  graph.addVertex(1);
  graph.addVertex(2);
  graph.addVertex(3);

  // add vertices (check insertion result)
  auto [v1, i1] = graph.addVertex(1);
  auto [v2, i2] = graph.addVertex(2);
  auto [v3, i3] = graph.addVertex(3);
  std::cout << "Number of vertices: " << graph.numVertices() << "\n";
  auto [v4, i4] = graph.addVertex(4);
  auto [v5, i5] = graph.addVertex(5);

  graph.addEdge(1, 3, 5);
  graph.updateEdge(1, 3, 10);
  graph.addEdge(2, 3, 15);
  graph.addEdge(4, 2, 52);
  graph.addEdge(5, 3, 53);

  if (graph.hasVertex(5))
    std::cout << "Vertex 5 exists.\n";
  if (graph.hasVertex(6))
    std::cout << "Vertex 6 exists.\n";

  std::cout << "Does edge (5,3) exists: " << graph.getEdge(5, 3).second
            << "\n"; // Check edge existence
  graph.removeEdge(5, 3);
  std::cout << "Does edge (5,3) exists: " << graph.getEdge(5, 3).second
            << "\n"; // Check edge existence after removal

  std::cout << "Number of vertices: " << graph.numVertices()
            << "\n"; // Number of vertices before clearing
  std::cout << "Number of edges: " << graph.numEdges()
            << "\n"; // Number of edges before clearing

  graph.clearEdges();

  std::cout << "Number of vertices: " << graph.numVertices()
            << "\n"; // Number of vertices after clearing
  std::cout << "Number of edges: " << graph.numEdges()
            << "\n"; // Number of edges after clearing

  // add weighted edge (returns {{src,dst,weight}, inserted})
  auto [weKey, edgeInserted] = graph.addEdge(1, 3, 5);
  if (edgeInserted) {
    auto [src, dst, w] = weKey;
    std::cout << "Added edge " << src << "->" << dst << " weight=" << w << "\n";
  } else
    std::cout << "Failed to add edge 1->3 (maybe already exists)\n";

  // updateEdge returns {previousWeight, updatedFlag}
  auto [prevW, updated] = graph.updateEdge(1, 3, 10);
  if (updated)
    std::cout << "updateEdge succeeded. previous weight = " << prevW << "\n";
  else
    std::cout << "updateEdge failed for 1->3\n";

  std::cout << "Number of vertices: " << graph.numVertices() << "\n";

  // ---------------- Graph 2 (unweighted) ----------------
  GraphCreationOptions opts1({GraphCreationOptions::Directed});
  CinderGraph<int, Unweighted> unweighted_graph(opts1);

  unweighted_graph.addVertex(1);
  unweighted_graph.addVertex(2);
  unweighted_graph.addVertex(3);
  unweighted_graph.addVertex(4);

  // addEdge for unweighted graph -> returns {{src,dest}, inserted}
  auto [ueKey, uadded] = unweighted_graph.addEdge(1, 2);
  if (uadded) {
    auto [usrc, udst] = ueKey;
    std::cout << "Added unweighted edge " << usrc << "->" << udst << "\n";
  } else {
    std::cout << "Failed to add unweighted edge 1->2\n";
  }

  std::cout << "Number of vertices (unweighted): "
            << unweighted_graph.numVertices() << "\n";

  // ---------------- Graph 3 (custom vertex/edge types) ----------------
  GraphCreationOptions options({GraphCreationOptions::Undirected});
  ListVertex lv1(1);
  ListVertex lv2(2);
  ListEdge e1(0.5f);
  ListEdge e2(0.8f);

  // note: use 'options' (Undirected) here
  CinderGraph<ListVertex, ListEdge> listGraph(options);
  listGraph.addVertex(lv1);
  listGraph.addVertex(lv2);

  // add weighted edge with ListEdge
  auto [listWeKey, listAdded] = listGraph.addEdge(lv1, lv2, e1);
  if (listAdded)
    std::cout << "Added ListEdge between lv1 and lv2 with weight "
              << e1.edge_weight << "\n";
  else
    std::cout << "Failed to add ListEdge lv1->lv2\n";

  // getEdge returns pair<optional<Edge_t>, bool>
  auto [maybeBeforeEdge, foundBefore] = listGraph.getEdge(lv1, lv2);
  if (foundBefore && maybeBeforeEdge)
    std::cout << "Edge value before update: " << maybeBeforeEdge->edge_weight
              << "\n";
  else
    std::cout << "Edge not found before update\n";

  std::cout << "Updating Edge Value to 0.8\n";

  auto [prevListW, listUpdated] = listGraph.updateEdge(lv1, lv2, e2);
  if (listUpdated)
    std::cout << "updateEdge succeeded. previous weight = "
              << prevListW.edge_weight << "\n";
  else
    std::cout << "updateEdge failed for lv1->lv2\n";

  auto [maybeAfterEdge, foundAfter] = listGraph.getEdge(lv1, lv2);
  if (foundAfter && maybeAfterEdge)
    std::cout << "Edge value after the update: " << maybeAfterEdge->edge_weight
              << "\n";
  else
    std::cout << "Edge not found after update\n";

  return 0;
}

#include "CinderPeak.hpp"
#include <iostream>
#include <string>

using namespace CinderPeak;

int main() {
  CinderGraph<std::string, double> g; // Weighted graph

  // ====== BASIC ADDITION ======
  g.addVertex("A");
  g.addVertex("B");
  g.addVertex("C");

  g["A"]["B"] = 1.5; // Add A->B edge with weight 1.5
  g["B"]["C"] = 2.5; // Add B->C
  g["A"]["C"] = 3.0; // Add A->C

  std::cout << "Graph initialized with 3 edges.\n";

  // ====== READ EDGE WEIGHT ======
  std::cout << "Weight A->B: " << g["A"]["B"] << "\n";
  std::cout << "Weight A->C: " << g["A"]["C"] << "\n";

  // ====== UPDATE EXISTING EDGE ======
  g["A"]["B"] = 9.99;
  std::cout << "Updated A->B: " << g["A"]["B"] << "\n";

  // ====== CHAINED EDGE ADDITIONS ======
  g["C"]["A"] = 5.5;
  g["C"]["B"] = 6.6;

  std::cout << "Added C->A and C->B.\n";

  // ====== CONDITIONAL CHECK (safe existence test) ======
  if (g["B"]["C"]) {
    std::cout << "Edge B->C exists, weight = " << g["B"]["C"] << "\n";
  }

  // ====== HANDLING MISSING EDGES ======
  try {
    std::cout << "Edge D->E weight: " << g["D"]["E"] << "\n"; // Will throw
  } catch (const std::runtime_error &e) {
    std::cout << "Caught: " << e.what() << "\n";
  }

  // ====== VERTEX AUTO-INSERTION (OPTIONAL IDEA) ======
  // Could extend to automatically insert vertices if missing.
  // Example:
  // g["X"]["Y"] = 4.4; // Auto-add vertices X, Y before adding edge

  // ====== STATISTICS + SANITY CHECK ======
  std::cout << "Graph has " << g.numVertices() << " vertices and "
            << g.numEdges() << " edges.\n";

  // ====== CLEANUP ======
  g.removeEdge("A", "C");
  if (!g["A"]["C"])
    std::cout << "Edge A->C removed.\n";

  return 0;
}

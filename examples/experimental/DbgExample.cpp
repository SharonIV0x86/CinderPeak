#include "CinderPeak.hpp"
#include <iostream>
#include <optional>
#include <string>

using namespace CinderPeak;

int main() {
  std::cout << "=== CinderPeak dbg() Utility Examples ===\n\n";

  // 1. Primitive types
  int int_val = 42;
  double dbl_val = 3.14159;
  bool bool_val = true;
  std::cout << "Integer: " << dbg(int_val) << "\n";
  std::cout << "Double:  " << dbg(dbl_val) << "\n";
  std::cout << "Boolean: " << dbg(bool_val) << "\n";

  // 2. std::string
  std::string str_val = "Hello Graph";
  std::cout << "String:  " << dbg(str_val) << "\n";

  // 3. Unweighted edge
  Unweighted unweighted_edge;
  std::cout << "Unweighted edge: " << dbg(unweighted_edge) << "\n";

  // 4. std::optional
  std::optional<int> opt_val = 99;
  std::optional<int> empty_opt;
  std::cout << "Optional (has value): " << dbg(opt_val) << "\n";
  std::cout << "Optional (empty):     " << dbg(empty_opt) << "\n";

  // 5. CinderVertex (has __id_)
  CinderVertex v1("Vertex A");
  CinderVertex v2("Vertex B");
  std::cout << "CinderVertex 1: " << dbg(v1) << "\n";
  std::cout << "CinderVertex 2: " << dbg(v2) << "\n";

  // 6. User-defined struct (fallback to address)
  struct CustomData {
    float x, y;
  };
  CustomData data{1.0f, 2.0f};
  std::cout << "Opaque struct: " << dbg(data) << "\n";

  return 0;
}
